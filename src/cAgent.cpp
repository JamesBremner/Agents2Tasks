#include "Agents2Tasks.h"

cAgent::cAgent(const std::vector<std::string> &vtoken)
    : myAssigned(false),
      myAssignedCount(0),
      myfGroup(false)
{
    if (vtoken.size() < 4)
        throw std::runtime_error(
            "18 Badly formatted agent");

    // parse agent name
    if (find(vtoken[1]))
        throw std::runtime_error("12	Duplicate agent name " + vtoken[1]);
    myName = vtoken[1];

    // parse family

    auto it = std::find(
        theDataStore.vFamily.begin(), theDataStore.vFamily.end(), vtoken[3]);
    if (it == theDataStore.vFamily.end())
    {
        // new family
        theDataStore.vFamily.push_back(vtoken[3]);
        myFamily = theDataStore.vFamily.size() - 1;
    }
    else
    {
        // member of an existing family
        myFamily = it - theDataStore.vFamily.begin();
    }

    parseTasks(4, vtoken);
}

cAgentGroup::cAgentGroup(
    const std::vector<std::string> &vtoken)
{
    myfGroup = true;
    myName = vtoken[1] + "_group";
    if (find(myName))
        throw std::runtime_error("12	Duplicate group name " + myName);
    myAssigned = false;
    myAssignedCount = 0;
    myFamily = -1;

    myTasks.push_back(
        std::make_pair(vtoken[2], 0));

    // store member agents
    for (int k = 3; k < vtoken.size(); k++)
    {
        auto *pa = cAgent::find(vtoken[k]);
        if (!pa)
            throw std::runtime_error("25 Unspecified group member " + vtoken[k]);
        myMember.push_back(pa);
    }
}

std::vector<cAgent *>
cAgent::get()
{
    return theDataStore.theAgents;
}

void cAgent::parseTasks(int first, const std::vector<std::string> &vtoken)
{
    for (int k = first; k < vtoken.size(); k++)
    {
        myTasks.push_back(
            std::make_pair(
                vtoken[k],
                atof(vtoken[2].c_str())));
    }
}

cAgent *cAgent::find(const std::string &name)
{
    auto it = std::find_if(
        theDataStore.theAgents.begin(), theDataStore.theAgents.end(),
        [&](cAgent *pa)
        {
            return pa->name() == name;
        });
    if (it == theDataStore.theAgents.end())
        return 0;
    return *it;
}

std::string cAgent::text() const
{
    std::stringstream ss;

    ss << "a " << myName;
    if (myTasks.size())
        ss << " " << myTasks[0].second
           << " " << theDataStore.vFamily[myFamily];

    for (auto &tp : myTasks)
    {
        ss << " " << tp.first;
    }
    ss << "\n";

    return ss.str();
}

std::string cAgentGroup::text() const
{
    return "cAgentGroup::text() NYI\n";
}

std::string cAgentGroup::specText() const
{
    std::stringstream ss;

    ss
        << "g " << user_name()
        << " " << myTasks[0].first;

    for (auto &member : myMember)
        ss << " " << member->name();

    ss << "\n";

    return ss.str();
}

/// @brief start of blocked time
/// @param day integer rep of timeslot day e.g. 20231110
/// @return set time to 1 second after midnight of the morning of the assignment
///
/// we need to block re-assignment to the same task type for two complete days

timepoint_t
timepoint(int day)
{
    // setup timeinfo with current time
    // https://cplusplus.com/reference/ctime/mktime/

    std::tm *timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // convert timeinfo to 1 second after midnight of the morning of the assignment
    timeinfo->tm_year = day / 10000 - 1900;
    timeinfo->tm_mon = (day - (timeinfo->tm_year + 1900) * 10000) / 100;
    timeinfo->tm_mday = (day - (timeinfo->tm_year + 1900) * 10000 - timeinfo->tm_mon * 100);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 1;

    // convert timeinfo to std::chrono timepoint
    std::time_t tt = std::mktime(timeinfo);
    if (tt == -1)
        throw std::runtime_error("20 Cannot parse timeslot timestamp");
    return std::chrono::system_clock::from_time_t(tt);
}

void cAgent::assign(int day)
{
    myAssigned = true;
    myAssignedCount++;
    myLastAssignmentTime = timepoint(day);
}

bool cAgent::cando(const std::string &type) const
{
    for (auto &pt : myTasks)
        if (pt.first == type)
            return true;
    return false;
}

bool cAgent::isAssignedRecently(
    int day) const
{
    /*
    Assigning an agent should block another assignment of that agent
    for two full days - the rest of the day assigned and all of the next day.

    This is implemented by backdating the first assignent and the potential assignment being testes
    to the previous midnight, so the exact time of day of the assignment is irrelevant.

    Example:

    0830 Mon -> 00000 Monday
    1000 Tue -> 0000 Tuesday Delta: 24 hours BLOCKED
    0830 Wed -> 0000 Wed Delta: 48 hours OK

     https://github.com/JamesBremner/Agents2Tasks/issues/8
     https://github.com/JamesBremner/Agents2Tasks/issues/23

     */
    const int full_days_blocked = 2;
    const int hours_blocked = 24 * (full_days_blocked - 1);

    if (
        std::chrono::duration_cast<std::chrono::hours>(
            timepoint(day) - myLastAssignmentTime)
            .count() > hours_blocked)
        return false;

    return true;
}

bool cAgentGroup::isAssignedRecently(int day) const
{
    if (cAgent::isAssignedRecently(day))
        return true;

    // check for blocked group members
    for (auto *member : myMember)
    {
        if (member->isAssignedRecently(day))
            return true;
    }
    return false;
}
void cAgent::sort(const cSlot *slot)
{
    // std::cout << "before sort\n";
    // for (cAgent *pa : cAgent::get())
    //     std::cout << pa->name() << "\n";

    cAgent::sortAssignedCount();
    cAgent::sortFamily(slot);

    // std::cout << "after sort\n";
    // for (cAgent *pa : cAgent::get())
    //     std::cout << pa->name() << "\n";
}

void cAgent::sortAssignedCount()
{
    std::stable_sort(
        theDataStore.theAgents.begin(), theDataStore.theAgents.end(),
        [](cAgent *a, cAgent *b)
        {
            return a->assignedCount() < b->assignedCount();
        });
}

void cAgent::sortFamily(const cSlot *slot)
{
    std::stable_sort(
        theDataStore.theAgents.begin(), theDataStore.theAgents.end(),
        [&](cAgent *a, cAgent *b)
        {
            bool af = slot->hasFamily(a->family());
            bool bf = slot->hasFamily(b->family());
            return (af && (!bf));
        });
}

void cAgent::unassignAll()
{
    for (cAgent *pa : theDataStore.theAgents)
        pa->unAssign();
}

std::vector<cAgent *>
cAgent::getForTask(const cTask &task)
{
    std::vector<cAgent *> ret;
    for (cAgent *pa : theDataStore.theAgents)
        if (pa->cando(task.type()))
            ret.push_back(pa);
    return ret;
}

std::string cAgent::specTextAll()
{
    std::string ret;
    for (auto *pa : theDataStore.theAgentsInputOrder)
        ret += pa->specText();

    return ret;
}

void cAgent::clear()
{
    for (auto *pa : theDataStore.theAgents)
        delete pa;
    theDataStore.theAgents.clear();
}

void cAgent::saveInputOrder()
{
    theDataStore.theAgentsInputOrder = theDataStore.theAgents;
}
void cAgent::restoreInputOrder()
{
    theDataStore.theAgents = theDataStore.theAgentsInputOrder;
}

bool cAgent::isSane()
{
    // allow agents to have membership in multiple groups
    // https://github.com/JamesBremner/Agents2Tasks/issues/39

    return true;
}