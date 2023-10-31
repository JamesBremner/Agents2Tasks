#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include "allocator.h"

cAgent::cAgent(
    const std::string &name,
    const std::vector<int> &vt,
    double cost)
    : myName(name)
{
    for (int t : vt)
    {
        myTasks.push_back(std::make_pair(t, cost));
    }
}

std::string cAgent::text(
    const std::vector<std::string> vTaskType) const
{
    std::stringstream ss;
    ss << myName << " ready for: ";
    for (auto &tsk : myTasks)
        ss << vTaskType[tsk.first]
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
std::string cSlot::text(
    const std::vector<cTask> &vTask,
    const std::vector<std::string> &vTaskType) const
{
    std::stringstream ss;
    ss << myName << " tasks: ";
    for (int tsk : myTasks)
        ss << vTaskType[vTask[tsk].myTaskType] << " ";
    return ss.str();
}

void cAllocator::clear()
{
    myAgents.clear();
    // myTasks.clear();
    myTaskType.clear();
    mySlot.clear();
    mySolution.clear();
}
void cAllocator::addAgent(
    const std::string &name,
    const std::string &canDoTaskTypes,
    double cost)
{
    int iAgent;
    if (isAgent(name, iAgent))
        throw std::runtime_error(
            "Duplicate agent name" + name);

    myAgents.emplace_back(
        name,
        taskTypeIndices(canDoTaskTypes),
        cost);
}

void cAllocator::addTaskType(
    const std::string &stype)
{
    if (std::find(
            myTaskType.begin(), myTaskType.end(), stype) != myTaskType.end())
        return;
    myTaskType.push_back(stype);
}

void cAllocator::addSlot(
    const std::string &name,
    const std::string &sTaskTypeName)
{
    if (isSlot(name))
        throw std::runtime_error(
            "Duplicate slot name" + name);

    // loop over tasks in slot
    std::vector<int> vTaskIndex;
    std::stringstream sst(sTaskTypeName);
    std::string a;
    while (getline(sst, a, ' '))
    {
        int taskIndex = myTask.end() - myTask.begin();
        myTask.emplace_back(
            findTaskType(a),
            taskIndex);
        vTaskIndex.push_back(taskIndex);
    }

    mySlot.emplace_back(
        name,
        vTaskIndex);
}

bool cAllocator::isAgent(
    const std::string &name,
    int &iAgent) const
{
    auto it = std::find_if(
        myAgents.begin(), myAgents.end(),
        [name](const cAgent &ag)
        {
            return (ag.name() == name);
        });
    if (it == myAgents.end())
    {
        iAgent = -1;
        return false;
    }
    iAgent = it - myAgents.begin();
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
    const std::string &sTask)
{
    std::vector<int> vTaskIndices;
    std::stringstream sst(sTask);
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
        ss << slot.text(
                  myTask,
                  myTaskType)
           << "\n";
    }

    ss << "\nSolution\n==========\n";
    for (int slot = 0; slot < mySolution.size(); slot++)
    {
        double cost = 0;
        ss << mySlot[slot].name() << "\n";
        for (auto &edge : mySolution[slot].edgeList())
        {
            int iAgent;
            isAgent(mySolution[slot].userName(edge.first), iAgent);
            cost += myAgents[iAgent].cost();

            ss << mySolution[slot].userName(edge.first)
               << " does " << mySolution[slot].userName(edge.second)
               << "\n";
        }
        ss << "Cost " << cost << "\n++++++++++\n";
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

void cAllocator::allocateMaxFlow()
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
            for (int kag = 0; kag < myAgents.size(); kag++)
            {
                if (myAgents[kag].isAble(myTask[task].myTaskType))
                {
                    // add link from agent to task agent is able to do
                    g.add(
                        myAgents[kag].name(),
                        "task" + std::to_string(task));
                }
            }
        }

        // apply pathfinder maximum flow allocation algorithm to the timeslot
        raven::graph::sGraphData gd;
        gd.g = g;
        mySolution.push_back(alloc(gd));
    }
}

void cAllocator::allocateHungarian()
{
    // mySolution.clear();

    // // loop over timeslots
    // for (auto &slot : mySlot)
    // {
    //     // construct cost martix
    //     // each agent gets a row, each task gets a column

    //     std::vector<std::vector<double>> costMatrix;
    //     for (auto &agent : myAgents)
    //     {
    //         std::vector<double> row;
    //         double delta = 0;
    //         for (auto &task : slot)
    //         {
    //             if (!agent.isAble(task.myTaskType))
    //                 row.push_back(100);
    //             else
    //             {
    //                 row.push_back(agent.cost() + delta);

    //                 // we have assumed that each agent costs the same
    //                 // no matter which task they do
    //                 // The Hungarian algorithm needs a different cost for each task
    //                 delta += 0.01;
    //             }
    //         }
    //         costMatrix.push_back(row);
    //     }

    //     // Subtract minimum cost in row from each cost in row
    //     bool fsolve = true;
    //     for (auto &row : costMatrix)
    //     {
    //         double rowMin = INT_MAX;
    //         for (int cost : row)
    //         {
    //             if (cost < rowMin)
    //                 rowMin = cost;
    //         }
    //         int countZero = 0;
    //         for (double &cost : row)
    //         {
    //             cost -= rowMin;
    //             if (cost < 0.00001)
    //                 countZero++;
    //         }
    //         // check that there is just one zero in the row
    //         if (countZero > 1)
    //             fsolve = false;
    //     }
    //     if (!fsolve)
    //         throw std::runtime_error(
    //             "Hungarian algorithm failed");

    //     // assign agent to task where modified cost is zero

    //     raven::graph::cGraph g;
    //     g.directed();
    //     for (int kagent = 0; kagent < myAgents.size(); kagent++)
    //     {
    //         for (int ktask = 0; ktask < slot.taskCount(); ktask++)
    //         {
    //             if (costMatrix[kagent][ktask] < 0.000001)
    //             {
    //                 // asignment found

    //                 g.add(
    //                     myAgents[kagent].name(),
    //                     myTaskType[ktask]);
    //             }
    //         }
    //     }
    //     mySolution.push_back(g);
    // }
}