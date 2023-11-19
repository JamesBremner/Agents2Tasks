#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include "allocator.h"

std::vector<std::string> cTask::vTaskType;


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
    for (const cAgent2Task &a2t : myAssigns[slotIndex])
    {
        ss << a2t.myAgent
           << " does " << a2t.myTask
           << "\n";
    }
    ss << "\nCost " << mySlotCost[slotIndex] << "\n";

    return ss.str();
}

std::string cAssigns::textFile(const char cid) const
{
    std::stringstream ss;
    int slotID = 0;
    for (auto &slot : myAssigns)
    {
        for (const cAgent2Task &a2t : slot)
        {
            ss << cid
               << " " << mySlotName[slotID]
               << " " << a2t.myAgent;
            if (a2t.myGroup)
                ss << " in group " << a2t.myGroup->name();
            ss << " to " << a2t.myTask << "\n";
        }
        slotID++;
    }
    return ss.str();
}

void cAssigns::writeFile(
    std::ofstream &ofs,
    const char cid) const
{
    ofs << textFile(cid);
}

void cAllocator::clear()
{
    for (cAgent *pa : myAgent)
        delete pa;
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
    double cost,
    const std::string &family)
{
    int iAgent;
    if (isAgent(name, iAgent))
        throw std::runtime_error("12 "
                                 "Duplicate agent name" +
                                 name);

    /* Construct new agent and store pointer to it

    */
    myAgent.push_back(
        new cAgent(
            *this,
            name,
            taskTypeIndices(canDoTaskTypes),
            cost,
            family));
}

void cAllocator::addAgentGroup(
    const std::vector<std::string> &vtoken)
{
    myAgent.push_back(
        new cAgentGroup(
            *this,
            vtoken,
            taskTypeIndices(vtoken[1])[0]));
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
                                 "Duplicate slot name " +
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

bool cAllocator::isAgentSane()
{
    std::set<std::string> groupMembers;
    for( auto* agent : myAgent )
    {
        if( agent->name().find("_group") == -1 ) {
            // ignore single agents
            continue;
        }
        for( auto& member : ((cAgentGroup*)agent)->getMembers() )
        {
            // check if agent specified in a previous group
            if( ! groupMembers.insert( member ).second )
                throw std::runtime_error("23 Dulplicate group member " + member );
        }
    }
    return true;
}

bool cAllocator::isSlotSane()
{
    if (!mySlot.size())
        throw std::runtime_error("22 No timeslots specified");

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
        [name](const cAgent *ag)
        {
            return (ag->name() == name);
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
    for (auto *agent : myAgent)
    {
        ss << agent->text() << "\n\n";
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
    for (cAgent2Task &a2t : solution)
    {
        int iAgent;
        isAgent(a2t.myAgent, iAgent);
        cost += myAgent[iAgent]->cost();
    }
    return cost;
}

std::vector<int> cAllocator::findAgentsForTask(int task)
{
    std::vector<int> ret;
    for (int kag = 0; kag < myAgent.size(); kag++)
    {
        if (myAgent[kag]->isAble(task))
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
                if (myAgent[kag]->isAble(myTask[task].taskType()))
                {
                    // add link from agent to task agent is able to do
                    g.add(
                        myAgent[kag]->name(),
                        "task" + std::to_string(task));

                    std::cout << myAgent[kag]->name() << " can do " << task << "\n";
                }
            }
        }

        // apply pathfinder maximum flow allocation algorithm to the timeslot
        raven::graph::cGraph sg;
        try
        {
            raven::graph::sGraphData gd;
            gd.g = g;
            sg = alloc(gd);
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("19 max flow error in PathFinder library");
        }

        slotsolution_t slotsolution;

        // loop over assignments
        for (auto &e : sg.edgeList())
        {
            auto stask = getTaskTypeName(
                atoi(sg.userName(e.second).substr(4).c_str()));
            slotsolution.emplace_back(
                sg.userName(e.first), stask);
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

bool cAllocator::assign(
    cAgent *pagent,
    cTask &task,
    cSlot &slot,
    slotsolution_t &slotsolution)
{
    if (!pagent)
        return false;

    // std::cout << "assigning " << pbestAgent->name() << " to " << task.typeName() << "\n";

    slot.assign(pagent->family());

    task.assign();

    pagent->assignTask(
        slot.day(),
        task.typeName(),
        slotsolution);

    return true;
}

void cAllocator::sortAgents(
    const cSlot &slot)
{
    theLog("\n=> sortAgents\n");
    agentsLog();

    // prefer to assign agents that have the least previous workload
    std::stable_sort(
        myAgent.begin(), myAgent.end(),
        [](const cAgent *a, const cAgent *b)
        {
            return (a->assignedCount() < b->assignedCount());
        });

    /* prefer to assign agents that have family members already assigned to the slot

    This sort is done second because family grouping is the higher priority
    https://github.com/JamesBremner/Agents2Tasks/issues/21#issuecomment-1813504598

    The stable sort ensures that agents in the same group but with the least previous workload will be assigned.
    */

    std::stable_sort(
        myAgent.begin(), myAgent.end(),
        [&](const cAgent *a, const cAgent *b)
        {
            return slot.hasFamily(a->family());
        });

    theLog("\n<= sortAgents\n");
    agentsLog();
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

void cAllocator::agentsLog()
{
    for (auto *a : myAgent)
        theLog(a->logText());
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
        a->writefile(ofs, allocator);

    for (auto &t : allocator.getconstSlots())
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
