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
    myTask.clear();
    myTaskType.clear();
    mySlot.clear();
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

std::string cAllocator::textProblem() const
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

    return ss.str();
}

std::string cAllocator::textSolution(
    const solution_t &solution) const
{
    std::stringstream ss;
    ss << "Solution\n==========\n";
    for (int slot = 0; slot < solution.size(); slot++)
    {
        double cost = 0;
        ss << mySlot[slot].name() << "\n";
        for (auto &edge : solution[slot].edgeList())
        {
            int iAgent;
            isAgent(solution[slot].userName(edge.first), iAgent);
            cost += myAgents[iAgent].cost();

            auto stask = solution[slot].userName(edge.second).substr(4);
            stask = myTaskType[myTask[atoi(stask.c_str())].myTaskType];

            ss << solution[slot].userName(edge.first)
               << " does " << stask
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
        mySolutionMaxflow.push_back(alloc(gd));
    }
}

cHungarian::cHungarian(
    cAllocator& allocator,
    cSlot &slot)
    : maxZero(0.00001)
{
    const double unablePenalty = 1000;

    for( int taskid : slot )
    {
        myTaskType.push_back( allocator.getTaskTypeName(taskid));
    }

    int iag = 0;
    for (auto &agent : allocator.getAgents() )
    {
        myAgent.push_back(iag++);

        std::vector<double> row;
        double delta = 0;
        for (int task : slot)
        {
            if (!agent.isAble(allocator.getTaskTypeID(task)))
                row.push_back(unablePenalty);
            else
            {
                row.push_back(agent.cost());

                // we have assumed that each agent costs the same
                // no matter which task they do
                // The Hungarian algorithm needs a different cost for each task
                delta += 0.01;
            }
        }
        myMxCost.push_back(row);
    }
    rowSubtract();
}

void cHungarian::rowSubtract()
{
    for (auto &row : myMxCost)
    {
        double min = INT_MAX;
        for (double c : row)
        {
            if (c < min)
            {
                min = c;
            }
        }
        for (double &c : row)
            c -= min;
    }
}

bool cHungarian::isSolvable()
{
    if( myMxCost.size() == 1 )
        return false;

    for (auto &row : myMxCost)
    {
        int rowZeroCount = 0;
        for (double c : row)
        {
            if (c < maxZero)
            {
                rowZeroCount++;
                if (rowZeroCount > 1)
                    return false;
            }
        }
    }
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        int colZeroCount = 0;
        for (int row = 0; row < myMxCost.size(); row++)
        {
            if (myMxCost[row][col] < maxZero)
            {
                colZeroCount++;
                if (colZeroCount > 1)
                    return false;
            }
        }
    }
    return true;
}

bool cHungarian::isFinished() const
{
    if( ! myMxCost.size() ) 
        return true;                // all agents assigned
    return ( ! myMxCost[0].size() );    // check for all task assigned
}

std::pair<int, std::string> cHungarian::AssignReduce()
{
    std::pair<int, std::string> retAgentTask;
    if (myMxCost.size() == 1)
    {
        // assign last task to last agent
        retAgentTask.first = myAgent[0];
        retAgentTask.second = myTaskType[0];
        myMxCost.clear();
        return retAgentTask;
    }
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        if (myMxCost[0][col] < maxZero)
        {
            retAgentTask.first = myAgent[0];
            retAgentTask.second = myTaskType[col];

            if (myMxCost.size() > 1)
            {
                std::vector<std::vector<double>> tmp;
                for (int row = 1; row < myMxCost.size(); row++)
                {
                    std::vector<double> vr;
                    for (int c = 0; c < myMxCost[0].size(); c++)
                    {
                        if (c != col)
                            vr.push_back(myMxCost[row][c]);
                    }
                    tmp.push_back(vr);
                }
                myMxCost = tmp;

                myAgent.erase(myAgent.begin());
                myTaskType.erase(myTaskType.begin() + col);
            }

            return retAgentTask;
        }
    }
    throw std::runtime_error(
        "cHungarian::AssignReduce failed");
}

void cAllocator::hungarian()
{
    mySolutionHungarian.clear();

    // loop over timeslots
    for (auto &slot : mySlot)
    {
        raven::graph::cGraph g;
        g.directed();

        // std::cout << "=========\n"
        //           << slot.name() << "\n";

        cHungarian H(
            *this,
            slot);

        while (!H.isFinished())
        {
            auto agentTask = H.AssignReduce();

            // std::cout << myAgents[agentTask.first].name()
            //           << " to " << agentTask.second
            //           << "\n";

            g.add(
                myAgents[agentTask.first].name(),
                agentTask.second );
        }
        mySolutionHungarian.push_back( g );
    }

}