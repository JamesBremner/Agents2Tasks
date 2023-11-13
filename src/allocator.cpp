#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include "allocator.h"

std::vector<std::string> cTask::vTaskType;

cAgent::cAgent(
    const std::string &name,
    const std::vector<int> &vt,
    double cost)
    : myName(name),
      myAssignedCount(0)
{
    for (int t : vt)
    {
        myTasks.push_back(std::make_pair(t, cost));
    }
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

/// @brief start of blocked time
/// @param day integer rep of timeslot day e.g. 20231110
/// @return set time to 1 second after midnight of the morning of the assignment
///
/// we need to block re-assignment to the same task type for two complete days

timepoint_t
timepoint(int day)
{
    std::tm timeinfo = std::tm();
    timeinfo.tm_year = day / 10000;
    timeinfo.tm_mon = (day - timeinfo.tm_year * 10000) / 100;
    timeinfo.tm_mday = (day - timeinfo.tm_year * 10000 - timeinfo.tm_mon * 100);
    timeinfo.tm_sec = 1;
    std::time_t tt = std::mktime(&timeinfo);
    return std::chrono::system_clock::from_time_t(tt);
}

void cAgent::assign(
    int day,
    const std::string &taskTypeName)
{
    fAssigned = true;
    myAssignedCount++;
    myAssignedDays.push_back(
        std::make_pair(timepoint(day), taskTypeName));
}

bool cAgent::isAssignedRecently(
    int day,
    const std::string &taskname) const
{
    // https://github.com/JamesBremner/Agents2Tasks/issues/8
    const int hours_blocked = 48;

    if (std::find_if(
            myAssignedDays.begin(), myAssignedDays.end(),
            [&, this](const std::pair<std::chrono::system_clock::time_point, std::string> prev_assign)
            {
                // check blocking period over
                if (std::chrono::duration_cast<std::chrono::hours>(timepoint(day) - prev_assign.first).count() > hours_blocked)
                    return false;

                // check task type is different
                if (prev_assign.second != taskname)
                    return false;

                // the assignment is blocked by a previous one
                return true;
            }) != myAssignedDays.end())
        return true;

    return false;
}

void cAgent::writefile(
    std::ofstream &ofs) const
{
    ofs << "a " << myName << " " << myTasks[0].second << " ";
    for (auto &it : myTasks)
    {
        ofs << cTask::typeName(it.first) << " ";
    }
    ofs << "\n";
}
std::string cSlot::text(
    const cAllocator &allocator) const
{
    std::stringstream ss;
    ss << myName << " tasks: ";
    for (int taskID : myTasks)
        ss << allocator.getTaskTypeName(taskID) << " ";
    return ss.str();
}

void cSlot::writefile(
    std::ofstream &ofs,
    const cAllocator &allocator) const
{
    ofs << "t " << myName << " ";
    for (int it : myTasks)
        ofs << allocator.getTaskTypeName(it) << " ";
    ofs << "\n";
}

void cAssigns::clear()
{
    myAssigns.clear();
    mySlotName.clear();
    mySlotCost.clear();
}

void cAssigns::add(
    const slotsolution_t &vps,
    const std::string &slotName,
    double cost)
{
    myAssigns.push_back(vps);
    mySlotName.push_back(slotName);
    mySlotCost.push_back(cost);
}

std::string cAssigns::text(
    int slotIndex) const
{
    std::stringstream ss;

    if (!myAssigns.size())
        return "No tasks assigned\n";
    if (!myAssigns[slotIndex].size())
        return "No tasks assigned\n";

    double cost = 0;
    for (auto &edge : myAssigns[slotIndex])
    {
        // int iAgent;
        // isAgent(edge.first, iAgent);
        // cost += myAgents[iAgent].cost();

        ss << edge.first
           << " does " << edge.second
           << "\n";
    }
    ss << "\nCost " << mySlotCost[slotIndex] << "\n";

    return ss.str();
}

void cAssigns::writeFile(
    std::ofstream &ofs,
    const char cid) const
{
    int slotID = 0;
    for (auto &slot : myAssigns)
    {
        for (auto &ap : slot)
        {
            ofs << cid
                << " " << mySlotName[slotID]
                << " " << ap.first << " to " << ap.second << "\n";
        }
        slotID++;
    }
}

void cAllocator::clear()
{
    myAgent.clear();
    myTask.clear();
    cTask::clear();
    mySlot.clear();
}
void cAllocator::clearSolution()
{
    mySolutionAgents2Task.clear();
}
void cAllocator::addAgent(
    const std::string &name,
    const std::string &canDoTaskTypes,
    double cost)
{
    int iAgent;
    if (isAgent(name, iAgent))
        throw std::runtime_error("12 "
                                 "Duplicate agent name" +
                                 name);

    myAgent.emplace_back(
        name,
        taskTypeIndices(canDoTaskTypes),
        cost);
}

void cAllocator::addTaskType(
    const std::string &stype)
{
    cTask::addTaskType(stype);
}
void cTask::addTaskType(
    const std::string &stype)
{
    if (std::find(
            vTaskType.begin(), vTaskType.end(), stype) != vTaskType.end())
        return;
    vTaskType.push_back(stype);
}

void cAllocator::addSlot(
    const std::string &name,
    const std::string &sTaskTypeName)
{
    if (isSlot(name))
        throw std::runtime_error("13 "
                                 "Duplicate slot name" +
                                 name);

    // loop over tasks in slot
    std::vector<int> vTaskIndex;
    std::stringstream sst(sTaskTypeName);
    std::string a;
    while (getline(sst, a, ' '))
    {
        int taskIndex = myTask.end() - myTask.begin();
        myTask.emplace_back(
            cTask::findType(a),
            taskIndex);
        vTaskIndex.push_back(taskIndex);
    }

    mySlot.emplace_back(
        name,
        vTaskIndex);
}

bool cAllocator::isSlotSane()
{
    /* Convert timeslot names to 64 bit integers so that their order can be checked

    While pointers to integers, or to anything else, are 64 bits long, the integers themselves are 32 bits long on 64 bit compilers.

    If 64 bit integers are required, then they have to be explicitly declared using long long or int64.

    For some discussion on this https://stackoverflow.com/q/17489857/16582
    */
    long long prev = -1;

    for (auto &slot : mySlot)
    {
        long long t = atoll(slot.name().c_str());
        if (!t)
            throw std::runtime_error("10 "
                                     "Timeslot badly formatted " +
                                     slot.name());
        if (std::to_string(t) != slot.name())
            throw std::runtime_error("17 "
                                     "timeslot overflow");
        if (prev == -1)
        {
            prev = t;
            continue;
        }
        if (prev >= t)
            throw std::runtime_error("11 "
                                     "Timeslots out of order at " +
                                     slot.name());
    }
    return true;
}

bool cAllocator::isAgent(
    const std::string &name,
    int &iAgent) const
{
    auto it = std::find_if(
        myAgent.begin(), myAgent.end(),
        [name](const cAgent &ag)
        {
            return (ag.name() == name);
        });
    if (it == myAgent.end())
    {
        iAgent = -1;
        return false;
    }
    iAgent = it - myAgent.begin();
    return true;
}
bool cAllocator::isSlot(const std::string &name)
{
    return (std::find_if(
                mySlot.begin(), mySlot.end(),
                [name](const cSlot &sl)
                {
                    return (sl.name() == name);
                }) != mySlot.end());
}

int cTask::findType(const std::string &name)
{
    int tasktype;
    auto it = std::find(
        vTaskType.begin(), vTaskType.end(), name);
    if (it == vTaskType.end())
    {
        vTaskType.push_back(name);
        tasktype = (vTaskType.size() - 1);
    }
    else
    {
        tasktype = it - vTaskType.begin();
    }
    return tasktype;
}

std::vector<int>
cAllocator::taskTypeIndices(
    const std::string &sTask)
{
    std::vector<int> vTaskIndices;
    std::stringstream sst(sTask);
    std::string a;
    while (getline(sst, a, ' '))
        vTaskIndices.push_back(cTask::findType(a));
    return vTaskIndices;
}

std::string cTask::typeText()
{
    std::stringstream ss;
    for (auto &task : vTaskType)
        ss << task << " ";
    return ss.str();
}

std::string cAllocator::textProblem() const
{
    std::stringstream ss;

    ss << "Task types\n==========\n";
    ss << cTask::typeText() << "\n\n";

    ss << "Agents\n==========\n";
    for (auto &agent : myAgent)
    {
        ss << agent.text() << "\n\n";
    }

    ss << "Time slots\n==========\n";
    for (auto &slot : mySlot)
    {
        ss << slot.text(*this) << "\n";
    }

    return ss.str();
}

double cAllocator::slotCost(
    slotsolution_t &solution) const
{
    double cost = 0;
    for (auto &edge : solution)
    {
        int iAgent;
        isAgent(edge.first, iAgent);
        cost += myAgent[iAgent].cost();
    }
    return cost;
}

std::vector<int> cAllocator::findAgentsForTask(int task)
{
    std::vector<int> ret;
    for (int kag = 0; kag < myAgent.size(); kag++)
    {
        if (myAgent[kag].isAble(task))
            ret.push_back(kag);
    }
    return ret;
}

void cAllocator::maxflow()
{
    mySolutionMaxflow.clear();

    raven::graph::cGraph g;

    // loop over time slots
    for (auto &slot : mySlot)
    {
        // setup fresh graph for timeslot
        g.clear();
        g.directed();

        // loop over the tasks in timeslot
        for (int task : slot)
        {
            // loop over agents
            for (int kag = 0; kag < myAgent.size(); kag++)
            {
                if (myAgent[kag].isAble(myTask[task].taskType()))
                {
                    // add link from agent to task agent is able to do
                    g.add(
                        myAgent[kag].name(),
                        "task" + std::to_string(task));

                    std::cout << myAgent[kag].name() << " can do " << task << "\n";
                }
            }
        }

        // apply pathfinder maximum flow allocation algorithm to the timeslot
        raven::graph::sGraphData gd;
        gd.g = g;
        auto sg = alloc(gd);

        slotsolution_t slotsolution;

        // loop over assignments
        for (auto &e : sg.edgeList())
        {
            auto stask = getTaskTypeName(
                atoi(sg.userName(e.second).substr(4).c_str()));
            slotsolution.push_back(std::make_pair(
                sg.userName(e.first), stask));
        }

        // store the assignments for this slot
        mySolutionMaxflow.add(
            slotsolution,
            slot.name(),
            slotCost(slotsolution));
    }
}

void cAllocator::hungarian()
{
    mySolutionHungarian.clear();

    // loop over timeslots
    for (auto &slot : mySlot)
    {
        cHungarian H(
            *this,
            slot);

        auto slotSolution = H.assignAll();

        mySolutionHungarian.add(
            slotSolution,
            slot.name(),
            slotCost(slotSolution));
    }
}

void cAllocator::sortAgentsByAssignedCount()
{
    std::sort(
        myAgent.begin(), myAgent.end(),
        [](const cAgent &a, const cAgent &b)
        {
            return (a.assignedCount() < b.assignedCount());
        });
}

std::vector<int> cAllocator::sortTasksByAgentCount(
    const cSlot &slot)
{
    std::vector<std::pair<int, int>> task_agentCount;
    for (int &taskIndex : slot.getTasks())
    {
        task_agentCount.push_back(std::make_pair(
            taskIndex,
            findAgentsForTask(task(taskIndex).taskType()).size()));
    }

    std::sort(
        task_agentCount.begin(), task_agentCount.end(),
        [](const std::pair<int, int> &a, const std::pair<int, int> &b)
        {
            return (a.second < b.second);
        });

    std::vector<int> sortedTaskIndex;
    for (auto &pair : task_agentCount)
        sortedTaskIndex.push_back(pair.first);

    return sortedTaskIndex;
}

void cAllocator::example1()
{
    clear();
    addTaskType("teacher");
    addTaskType("cleaner");
    addTaskType("accountant");
    addAgent(
        "John",
        {"teacher cleaner"},
        3.0);
    addAgent(
        "Margaret",
        {"accountant cleaner"},
        4.0);
    addAgent(
        "Andrew",
        {"accountant teacher"},
        5.0);
    addSlot(
        "28/OCT/2023/8:30",
        {"teacher teacher cleaner"});
    addSlot(
        "29/OCT/2023/10:00",
        {"teacher accountant"});
    addSlot(
        "2/NOV/2023/8:30",
        {"teacher teacher accountant"});
    addSlot(
        "3/NOV/2023/10:00",
        {"teacher accountant"});
}

void writefile(
    const cAllocator &allocator,
    const std::string &fname)
{
    std::ofstream ofs(fname);
    if (!ofs.is_open())
        throw std::runtime_error("14 "
                                 "Cannot open output file");

    for (auto &a : allocator.getconstAgents())
        a.writefile(ofs);

    for (auto &t : allocator.getSlots())
        t.writefile(ofs, allocator);

    // mySolutionMaxflow.writeFile(
    //     ofs,
    //     'F');
    // mySolutionHungarian.writeFile(
    //     ofs,
    //     'H');
    allocator.getSolutionAgents2Task().writeFile(
        ofs,
        'A');
}
