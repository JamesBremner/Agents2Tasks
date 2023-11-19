#include <sstream>
#include "allocator.h"

std::vector<std::string> cAgent::vFamily;

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

cAgent::cAgent(
    cAllocator &A,
    const std::string &name,
    const std::vector<int> &vt,
    double cost,
    const std::string family)
    : allocator(A),
      myName(name),
      myAssignedCount(0)
{
    for (int t : vt)
    {
        myTasks.push_back(std::make_pair(t, cost));
    }
    auto it = std::find(
        vFamily.begin(), vFamily.end(), family);
    if (it == vFamily.end())
    {
        vFamily.push_back(family);
        myFamily = vFamily.size() - 1;
    }
    else
    {
        myFamily = it - vFamily.begin();
    }
}

cAgentGroup::cAgentGroup(
    cAllocator &A,
    const std::vector<std::string> &vtoken,
    int taskID)
    : cAgent(
          A,
          vtoken[2] + "_group",
          {taskID}, 0, "none")
{
    for (int k = 2; k < vtoken.size(); k++)
        add(vtoken[k]);
}

std::string cAgent::text() const
{
    std::stringstream ss;
    ss << myName << " ready for: ";
    for (auto &tsk : myTasks)
        ss << cTask::typeName(tsk.first)
           << " at cost " << tsk.second << " ";
    return ss.str();
}
bool cAgent::isAble(int task) const
{
    return (std::find_if(
                myTasks.begin(), myTasks.end(),
                [&](const std::pair<int, double> &cando)
                {
                    if (cando.first == task)
                        return true;
                    return false;
                }) != myTasks.end());
}

double cAgent::cost() const
{
    if (!myTasks.size())
        return 0;

    // assume cost same for all tasks
    return myTasks[0].second;
}



void cAgent::assignTask(
    int day,
    const std::string &taskTypeName,
    slotsolution_t &slotsolution)
{
    myAssign = eAssign::agent;
    myAssignedCount++;
    myLastAssignmentTime = timepoint(day);
    slotsolution.emplace_back(
        myName,
        taskTypeName);
}

void cAgentGroup::assignTask(
    int day,
    const std::string &taskTypeName,
    slotsolution_t &slotsolution)
{
    // assign the agent group
    cAgent::assignTask(day, taskTypeName, slotsolution);
    myAssign = eAssign::group;

    // assign the member agents
    for (auto &member : myAgent)
    {
        int ia;
        allocator.isAgent(member, ia);
        allocator.getAgents()[ia]->assignTask(
            day,
            taskTypeName,
            slotsolution);
        slotsolution.back().myGroup = this;
    }
}

bool cAgent::isAssignedRecently(
    int day,
    const std::string &taskname) const
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

bool cAgentGroup::isAssignedRecently(
    int day,
    const std::string &taskname) const
{
    if (cAgent::isAssignedRecently(day, taskname))
        return true;

    // check for blocked group members
    for (auto &agentName : myAgent)
    {
        int iAgent;
        if (allocator.isAgent(agentName, iAgent))
            if (allocator.getconstAgents()[iAgent]->isAssignedRecently(day, taskname))
                return true;
    }
    return false;
}

void cAgent::writefile(
    std::ofstream &ofs,
    const cAllocator &A) const
{
    ofs << "a " << myName << " " << myTasks[0].second
        << " " << vFamily[myFamily] << " ";
    for (auto &it : myTasks)
    {
        ofs << cTask::typeName(it.first) << " ";
    }
    ofs << "\n";
}
void cAgentGroup::writefile(
    std::ofstream &ofs,
    const cAllocator &A) const
{
    const auto vA = A.getconstAgents();
    ofs << "g " << cTask::typeName(myTasks[0].first);
    for (auto &member : myAgent)
    {
        ofs << " " << member;
    }

    ofs << "\n";
}

std::string cAgent::logText() const
{
    std::stringstream ss;
    ss << myName << " assigned " << isAssigned()
       << " count " << myAssignedCount
       << " family " << vFamily[myFamily]
       << "\n";
    return ss.str();
}
