#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <chrono>

#include "../../PathFinder/src/GraphTheory.h"

typedef std::vector<std::pair<std::string, std::string>> slotsolution_t;
typedef std::vector<slotsolution_t> solution_t;

class cAllocator;

#include "cAgent.h"

#include "cTask.h"

/// @brief A timeslot with several task that need agents to be assigned
class cSlot
{
    std::string myName;
    std::vector<int> myTasks; // indices of tasks in this slot
    std::set<int> myFamily;   // indices of family groups assigned to this slot

public:
    cSlot(
        const std::string &name,
        const std::vector<int> &vt)
        : myName(name),
          myTasks(vt)
    {
    }

    void assign(int iFamily)
    {
        myFamily.insert(iFamily);
    }

    std::string name() const
    {
        return myName;
    }

    /// @brief get integer representation for day containing timeslot
    /// @return integer day
    ///
    /// Assumes slot name format YYYYMMDDHHMM
    /// returns int( YYYYMMDD )

    int day() const
    {
        return atoi(myName.substr(0, 8).c_str());
    }

    std::string text(
        const cAllocator &allocator) const;

    int taskCount() const
    {
        return myTasks.size();
    }

    int taskID(int taskpos) const
    {
        return myTasks[taskpos];
    }

    std::vector<int> getTasks() const
    {
        return myTasks;
    }
    bool hasFamily(int iFamily) const
    {
        return (myFamily.find(iFamily) != myFamily.end());
    }

    std::vector<int>::iterator begin()
    {
        return myTasks.begin();
    }
    std::vector<int>::iterator end()
    {
        return myTasks.end();
    }

    void writefile(
        std::ofstream &ofs,
        const cAllocator &allocator) const;
};

/// @brief Assign agents to slot, minimizing cost, using Hungarian algorithm

class cHungarian
{
    const std::vector<cAgent> &myAgent;
    const std::vector<std::string> &myTaskType;

    std::vector<std::vector<double>> myMxCost; // cost matrix
    std::vector<int> myRowAgent;               // agent index in each row
    std::vector<int> myColTask;                // task type index in each column

    double maxZero;

    void rowSubtract();      // subtract min row value from ech column
    void colSubtract();      // subtract min col value from each row
    bool isSolvable();       // check for a single zero in each row and column
    bool isFinished() const; // check for all asignments completed

    /// @brief Assign agent to a task, remove agent and task from cost matrix
    /// @return string pair, agent name and task type

    std::pair<std::string, std::string> AssignReduce();

    int unAssignedAgentCount() const
    {
        return myMxCost.size();
    }
    int unAssignedTaskCount() const
    {
        if (!unAssignedAgentCount())
            return 0;
        return myMxCost[0].size();
    }

public:
    /// @brief Constructor
    /// @param allocator containing problem specification
    /// @param slot      slot to assign

    cHungarian(
        cAllocator &allocator,
        cSlot &slot);

    /// @brief Assign agents to all tasks
    /// @return agent name,task type name pairs for all assignments in timeslot

    slotsolution_t assignAll();
};

/// @brief Task assignment for all timeslots and one algorithm

class cAssigns
{
    solution_t myAssigns;
    std::vector<std::string> mySlotName;
    std::vector<double> mySlotCost;

public:
    void clear();

    void add(
        const slotsolution_t &slotsolution,
        const std::string &slotName,
        double cost);

    std::string text(
        int slotIndex) const;

    void writeFile(
        std::ofstream &ofs,
        const char cid) const;
};

enum class eOptimizer
{
    cost,
    family
};

/// @brief Assign agents to task in multiple independant slots

class cAllocator
{
    std::vector<cAgent> myAgent; // agents
    std::vector<cTask> myTask;   // defined tasks
    std::vector<cSlot> mySlot;   // slots containg tasks

    cAssigns mySolutionMaxflow;
    cAssigns mySolutionHungarian;
    cAssigns mySolutionAgents2Task;

    int mySlotCurrent;

    bool isAgent(
        const std::string &name,
        int &iAgent) const;
    bool isSlot(const std::string &name);

    std::vector<int>
    taskTypeIndices(
        const std::string &vTask);

    eOptimizer myOptimizer;

public:
    cAllocator()
        : mySlotCurrent(0),
          myOptimizer(eOptimizer::cost)
    {
    }

    void clear();
    void clearSolution();

    void addAgent(
        const std::string &name,
        const std::string &canDoTaskTypes,
        double cost,
        const std::string &family);

    void addTaskType(
        const std::string &stype);

    void addSlot(
        const std::string &name,
        const std::string &vTask);

    void example1();

    /// @brief Check sanity of timeslot names and order
    /// @return true if OK
    ///
    /// if not OK throws runtime_error exception

    bool isSlotSane();

    std::vector<int> findAgentsForTask(int task);

    void setSlotFirst()
    {
        mySlotCurrent = 0;
    }
    void setSlotPrev()
    {
        mySlotCurrent--;
        if (mySlotCurrent < 0)
            mySlotCurrent = 0;
    }
    void setSlotNext()
    {
        mySlotCurrent++;
        if (mySlotCurrent > mySlot.size() - 1)
            mySlotCurrent = mySlot.size() - 1;
    }
    void setSlotLast()
    {
        mySlotCurrent = mySlot.size() - 1;
    }

    void unassignAgents()
    {
        for (auto &a : myAgent)
            a.unAssign();
    }

    void sortAgents(const cSlot &slot);

    std::vector<int> sortTasksByAgentCount(
        const cSlot &slot);

    void add(
        const slotsolution_t &slotsolution,
        const std::string &slotName,
        double cost)
    {
        mySolutionAgents2Task.add(
            slotsolution,
            slotName,
            cost);
    }

    /// @brief Allocate agents to tasks using max flow algorithm

    void maxflow();

    /// @brief  Allocate agents to tasks using Hungarian algorithm
    /// https://en.wikipedia.org/wiki/Hungarian_algorithm

    void hungarian();

    /// @brief Assign agent to task
    /// @param agent
    /// @param slot

    void assign(
        cAgent &agent,
        cSlot &slot);

    std::vector<cAgent> &getAgents()
    {
        return myAgent;
    }
    const std::vector<cAgent> &getconstAgents() const
    {
        return myAgent;
    }
    std::vector<cSlot> &getSlots()
    {
        return mySlot;
    }
    const std::vector<cSlot> &getconstSlots() const
    {
        return mySlot;
    }
    const cAssigns &getSolutionAgents2Task() const
    {
        return mySolutionAgents2Task;
    }

    /// @brief getTaskTypeName
    /// @param i index of task
    /// @return type name

    std::string getTaskTypeName(int i) const
    {
        return myTask[i].typeName();
    }
    /// @brief getTaskTypeID
    /// @param i index of task
    /// @return type name id

    int getTaskTypeID(int i) const
    {
        return myTask[i].taskType();
    }

    cTask &task(int index)
    {
        return myTask[index];
    }

    std::string textProblem() const;

    std::string maxflowText() const
    {
        return mySolutionMaxflow.text(mySlotCurrent);
    }
    std::string hungarianText() const
    {
        return mySolutionHungarian.text(mySlotCurrent);
    }
    std::string agents2TasksText() const
    {
        return mySolutionAgents2Task.text(mySlotCurrent);
    }
    double slotCost(
        slotsolution_t &solution) const;

    std::string slotName() const
    {
        return mySlot[mySlotCurrent].name();
    }
};

// Declare free functions

void readfile(
    cAllocator &allocator,
    const std::string &fname);
void readstring(
    cAllocator &allocator,
    const std::string &sin);

void input(
    cAllocator &allocator,
    std::istream &ifs);

/// @brief Assign agents to tasks with constraints by Janusz Dalecki
/// @param allocator
///
/// https://github.com/JamesBremner/Agents2Tasks

void Janusz(
    cAllocator &allocator);

void writefile(
    const cAllocator &allocator,
    const std::string &fname);

bool unitTest();