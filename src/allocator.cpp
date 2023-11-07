#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include "allocator.h"

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
void cAgent::writefile(
    std::ofstream &ofs,
    const cAllocator &allocator)
{
    ofs << "a " << myName << " " << myTasks[0].second << " ";
    for (auto &it : myTasks)
    {
        ofs << allocator.getTaskTypeNameFromTypeID(it.first) << " ";
    }
    ofs << "\n";
}
std::string cSlot::text(
    const std::vector<cTask> &vTask,
    const cAllocator &allocator) const
{
    std::stringstream ss;
    ss << myName << " tasks: ";
    for (int it : myTasks)
        ss << allocator.getTaskTypeName(it) << " ";
    return ss.str();
}

void cSlot::writefile(
    std::ofstream &ofs,
    const cAllocator &allocator)
{
    ofs << "t " << myName << " ";
    for (int it : myTasks)
        ofs << allocator.getTaskTypeName(it) << " ";
    ofs << "\n";
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
                  *this)
           << "\n";
    }

    return ss.str();
}

std::string cAllocator::textSolution(
    const solution_t &solution) const
{
    std::stringstream ss;

    double cost = 0;
    for (auto &edge : solution[mySlotCurrent])
    {
        int iAgent;
        isAgent(edge.first, iAgent);
        cost += myAgents[iAgent].cost();

        ss << edge.first
           << " does " << edge.second
           << "\n";
    }
    ss << "\nCost " << cost << "\n";

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
        mySolutionMaxflow.push_back(slotsolution);
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

        mySolutionHungarian.push_back(H.assignAll());
    }
}

void cAllocator::agents2tasks()
{
    mySolutionAgents2Task.clear();

    for (auto &slot : mySlot)
    {
        slotsolution_t slotSolution;
        int agentsUnassignedCount = myAgents.size();
        int tasksUnassignedCount = slot.taskCount();

        for (auto &a : myAgents)
            a.assign(false);
        std::sort(
            myAgents.begin(), myAgents.end(),
            [](const cAgent &a, const cAgent &b)
            {
                return (a.assignedCount() < b.assignedCount());
            });

        // loop over slot tasks
        for (int &taskIndex : slot.getTasks())
        {
            cTask &task = myTask[taskIndex];

            // find cheapest unassigned agent that can do the task
            double bestCost = INT_MAX;
            cAgent *pbestAgent = 0;
            for (auto &agent : myAgents)
            {
                if (agent.isAssigned())
                    continue;
                if (!agent.isAble(task.myTaskType))
                    continue;
                double cost = agent.cost();
                if (cost < bestCost)
                {
                    bestCost = cost;
                    pbestAgent = &agent;
                }
            }
            if (!pbestAgent)
                continue;

            // assign cheapest agent
            slotSolution.push_back(std::make_pair(
                pbestAgent->name(),
                myTaskType[task.myTaskType]));
            myTask[taskIndex].fAssigned = true;
            tasksUnassignedCount--;
            pbestAgent->assign();
            agentsUnassignedCount--;

            if( ! agentsUnassignedCount )
                break;
        }
        mySolutionAgents2Task.push_back(slotSolution);
    }
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

void cAllocator::readfile(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open input file");

    clear();
    std::string line;
    while (getline(ifs, line))
    {
        std::vector<std::string> vtoken;
        std::stringstream sst(line);
        std::string a;
        while (getline(sst, a, ' '))
            vtoken.push_back(a);
        int p;

        switch (line[0])
        {
        case 'a':
            p = line.find(" ");
            p = line.find(" ", p + 1);
            p = line.find(" ", p + 1);
            addAgent(
                vtoken[1],
                line.substr(p + 1),
                atof(vtoken[2].c_str()));
            break;

        case 't':
            p = line.find(" ");
            p = line.find(" ", p + 1);
            addSlot(
                vtoken[1],
                line.substr(p + 1));
            break;
        }
    }
}
void cAllocator::writefile(const std::string &fname)
{
    std::ofstream ofs(fname);
    if (!ofs.is_open())
        throw std::runtime_error(
            "Cannot open input file");

    for (auto &a : myAgents)
        a.writefile(ofs, *this);

    for (auto &t : mySlot)
        t.writefile(ofs, *this);

    for (int slot = 0; slot < mySlot.size(); slot++)
    {
        for (auto &ap : mySolutionMaxflow[slot])
        {
            ofs << "F " << mySlot[slot].name()
                << " " << ap.first << " to " << ap.second << "\n";
        }
    }
    for (int slot = 0; slot < mySlot.size(); slot++)
    {
        for (auto &ap : mySolutionHungarian[slot])
        {
            ofs << "H " << mySlot[slot].name()
                << " " << ap.first << " to " << ap.second << "\n";
        }
    }
    for (int slot = 0; slot < mySlot.size(); slot++)
    {
        for (auto &ap : mySolutionAgents2Task[slot])
        {
            ofs << "A " << mySlot[slot].name()
                << " " << ap.first << " to " << ap.second << "\n";
        }
    }
}