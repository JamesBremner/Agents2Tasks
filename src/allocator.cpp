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

double cAllocator::slotCost(
    slotsolution_t &solution) const
{
    double cost = 0;
    for (auto &edge : solution)
    {
        int iAgent;
        isAgent(edge.first, iAgent);
        cost += myAgents[iAgent].cost();
    }
    return cost;
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

void cAllocator::agents2tasks()
{
    mySolutionAgents2Task.clear();

    for (auto &slot : mySlot)
    {
        slotsolution_t slotSolution;
        int agentsUnassignedCount = myAgents.size();
        int tasksUnassignedCount = slot.taskCount();

        // unassign all agents
        for (auto &a : myAgents)
            a.assign(false);

        /* Sort agents in ascending order of previous assignments

          So that if there are multiple agents of equal cost available to do a task,
          the task will be assigned to the agent with the fewest assignments in the previous timeslots.

          So the uneven distribution of work to agents when there are more agents than work
          and the agents have the same cost per assignment
          will decrease as a percentage of total assignments
          as the number of timeslots increases
          with a limit of +/- 1 assignments per agent.

          https://github.com/JamesBremner/Agents2Tasks/issues/5#issuecomment-1801948876

        */
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

            // check for all agents assigned
            if (!agentsUnassignedCount)
                break;
        }
        mySolutionAgents2Task.add(
            slotSolution,
            slot.name(),
            slotCost(slotSolution));
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
            "Cannot open output file");

    for (auto &a : myAgents)
        a.writefile(ofs, *this);

    for (auto &t : mySlot)
        t.writefile(ofs, *this);

    mySolutionMaxflow.writeFile(
        ofs,
        'F');
    mySolutionHungarian.writeFile(
        ofs,
        'H');
    mySolutionAgents2Task.writeFile(
        ofs,
        'A');
}
