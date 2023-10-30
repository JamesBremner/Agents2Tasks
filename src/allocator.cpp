#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "../../PathFinder/src/GraphTheory.h"
#include "allocator.h"

std::string cAgent::text(
    const std::vector<std::string> vTaskType) const
{
    std::stringstream ss;
    ss << myName << " ready for: ";
    for (int tsk : myTasks)
        ss << vTaskType[tsk] << " ";
    return ss.str();
}
bool cAgent::isAble(int task)
{
    return (std::find(
                myTasks.begin(), myTasks.end(), task) != myTasks.end());
}
std::string cSlot::text(
    const std::vector<std::string> vTaskType) const
{
    std::stringstream ss;
    ss << myName << " tasks: ";
    for (int tsk : myTasks)
        ss << vTaskType[tsk] << " ";
    return ss.str();
}

void cAllocator::clear()
{
    myAgents.clear();
    myTasks.clear();
    myTaskType.clear();
    mySlot.clear();
    mySolution.clear();
}
void cAllocator::addAgent(
    const std::string &name,
    const std::string &canDoTaskTypes)
{
    if (isAgent(name))
        throw std::runtime_error(
            "Duplicate agent name" + name);

    myAgents.emplace_back(
        name,
        taskTypeIndices(canDoTaskTypes));
}

void cAllocator::addTask(
    const std::string &stype)
{
    if (std::find(
            myTaskType.begin(), myTaskType.end(), stype) != myTaskType.end())
        return;
    myTaskType.push_back(stype);
}

void cAllocator::addSlot(
    const std::string &name,
    const std::string &vTask)
{
    if (isSlot(name))
        throw std::runtime_error(
            "Duplicate slot name" + name);

    mySlot.emplace_back(
        name,
        taskTypeIndices(vTask));
}

bool cAllocator::isAgent(const std::string &name)
{
    return (std::find_if(
                myAgents.begin(), myAgents.end(),
                [name](const cAgent &ag)
                {
                    return (ag.name() == name);
                }) != myAgents.end());
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

int cAllocator::findTaskType(const std::string &name)
{
    int task;
    auto it = std::find(
        myTaskType.begin(), myTaskType.end(), name);
    if (it == myTaskType.end())
    {
        myTaskType.push_back(name);
        task = (myTaskType.size() - 1);
    }
    else
    {
        task = it - myTaskType.begin();
    }
    return task;
}

std::vector<int>
cAllocator::taskTypeIndices(
    const std::string &vTask)
{
    std::vector<int> vTaskIndices;
    std::stringstream sst(vTask);
    std::string a;
    while (getline(sst, a, ' '))
        vTaskIndices.push_back(findTaskType(a));
    return vTaskIndices;
}

std::string cAllocator::text() const
{
    std::stringstream ss;

    ss << "Task types\n==========\n";
    for (auto &task : myTaskType)
        ss << task << " ";
    ss << "\n\n";

    ss << "Agents\n==========\n";
    for (auto &agent : myAgents)
    {
        ss << agent.text(myTaskType) << "\n\n";
    }

    ss << "Time slots\n==========\n";
    for (auto &slot : mySlot)
    {
        ss << slot.text(myTaskType) << "\n\n";
    }

    ss << "Solution\n==========\n";
    for (int slot = 0; slot < mySolution.size(); slot++)
    {
        ss << mySlot[slot].name() << "\n";
        for (int k = 0; k < mySolution[slot].size(); k += 2)
        {
            ss << mySolution[slot][k] << " does " << mySolution[slot][k + 1] << "\n";
        }
        ss << "++++++++++\n";
    }

    return ss.str();
}

std::vector<int> cAllocator::findAgentsForTask(int task)
{
    std::vector<int> ret;
    for (int kag = 0; kag < myAgents.size(); kag++)
    {
        if (myAgents[kag].isAble(task))
            ret.push_back(kag);
    }
    return ret;
}

void cAllocator::allocate()
{
    mySolution.clear();

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
            for (int agent : findAgentsForTask(task))
            {
                // add link from agent to task agent is able to do
                g.add(
                    myAgents[agent].name(),
                    myTaskType[task]);
            }
        }

        // apply pathfinder maximum flow allocation algorithm to the timeslot
        raven::graph::sGraphData gd;
        gd.g = g;
        mySolution.push_back(alloc(gd));
    }
}