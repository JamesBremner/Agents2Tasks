#include "Agents2Tasks.h"


cAgent::cAgent(const std::vector<std::string> &vtoken)
    : myName(vtoken[1]),
      myAssigned(false),
      myAssignedCount(0)
{
    // parse family

    auto it = std::find(
       theDataStore.vFamily.begin(),  theDataStore.vFamily.end(), vtoken[3]);
    if (it ==  theDataStore.vFamily.end())
    {
        // new family
         theDataStore.vFamily.push_back(vtoken[3]);
        myFamily =  theDataStore.vFamily.size() - 1;
    }
    else
    {
        // member of an existing family
        myFamily = it -  theDataStore.vFamily.begin();
    }

    parseTasks(4, vtoken);
}

cAgentGroup::cAgentGroup(
    const std::vector<std::string> &vtoken)
{
    myName = vtoken[2] + "_group";
    myAssigned = false;
    myAssignedCount = 0;
    myFamily = -1;

    cTask *task = cTask::find(vtoken[1]);
    if (!task)
        task = cTask::add(vtoken[1]);
    myTasks.push_back(
        std::make_pair(task, 0));

    // store member agents
    for (int k = 2; k < vtoken.size(); k++)
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
        cTask *pt = cTask::find(vtoken[k]);
        if (!pt)
            cTask::add(vtoken[k]);

        myTasks.push_back(
            std::make_pair(
                cTask::find(vtoken[k]),
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

    ss
        << "a " << myName
        << " " << myTasks[0].second
        << " " <<  theDataStore.vFamily[myFamily];

    for (auto &tp : myTasks)
    {
        ss << " " << tp.first->name();
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
        << "g " << myTasks[0].first->name();

    for (auto &member : myMember)
        ss << " " << member->name();

    ss << "\n";

    return ss.str();
}

void cAgent::add(const std::vector<std::string> &vtoken)
{
    if (find(vtoken[1]))
        throw std::runtime_error("12	Duplicate agent name " + vtoken[1]);

    theDataStore.theAgents.push_back(
        new cAgent(vtoken));
}

void cAgentGroup::add(const std::vector<std::string> &vtoken)
{
    if (find(vtoken[2] + "_group"))
        throw std::runtime_error("12	Duplicate group name " + vtoken[2]+ "_group");
    theDataStore.theAgents.push_back(
        new cAgentGroup(vtoken));
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

void cAgentGroup::assign(int day)
{
    // assign the agent group
    cAgent::assign(day);

    // assign the member agents
    for (auto &member : myMember)
    {
        member->assign(day);
    }
}

bool cAgent::cando(cTask *task) const
{
    for (auto &pt : myTasks)
        if (pt.first == task)
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
cAgent::getForTask(cTask *task)
{
    std::vector<cAgent *> ret;
    for (cAgent *pa : theDataStore.theAgents)
        if (pa->cando(task))
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