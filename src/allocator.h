#pragma once
#include <string>
#include <vector>

#include "../../PathFinder/src/GraphTheory.h"

typedef std::vector<std::pair<std::string, std::string>> slotsolution_t;
typedef std::vector<slotsolution_t> solution_t;

class cAllocator;

/// @brief An agent can be assigned to tasks of certain work types

class cAgent
{
    std::string myName;

    /// @brief tasks agent ready to do ( task type id, cost )
    std::vector<std::pair<int, double>> myTasks;

    bool fAssigned;     // true if assigned to task in current timeslot

    int myAssignedCount;    // task assignments in all timeslots

public:

    /// @brief Constructor
    /// @param name 
    /// @param vt       // Indices of task types agent can do
    /// @param cost     // Cost of assigning agent to any task

    cAgent(
        const std::string &name,
        const std::vector<int> &vt,
        double cost);

    std::string name() const
    {
        return myName;
    }

    /// @brief true if agent can do task
    /// @param task task type id
    /// @return

    bool isAble(int task) const;

    double cost() const;

    void assign( bool f = true )
    {
        fAssigned = f;
        if( f )
            myAssignedCount++;
    }
    bool isAssigned() const
    {
        return fAssigned;
    }

    int assignedCount() const
    {
        return myAssignedCount;
    }

    std::string text(
        const std::vector<std::string> vTaskType) const;

    void writefile(
        std::ofstream &ofs,
        const cAllocator &allocator);
};

/// @brief A task, unique to its timeslot, of a certain type
class cTask
{
public:
    int myTaskType;
    int myIndex;
    bool fAssigned;
    cTask(int iType, int i)
        : myTaskType(iType),
          myIndex(i),
          fAssigned(false)
    {
    }
};

/// @brief A timeslot with several task that need agents to be assigned
class cSlot
{
    std::string myName;
    std::vector<int> myTasks; // index of tasks in this slot

public:
    cSlot(
        const std::string &name,
        const std::vector<int> &vt)
        : myName(name),
          myTasks(vt)
    {
    }
    std::string name() const
    {
        return myName;
    }
    std::string text(
        const std::vector<cTask> &vTask,
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
        const cAllocator &allocator);
};

/// @brief Assign agents to slot, minimizing cost, using Hungarian algorithm

class cHungarian
{
    const std::vector<cAgent>& myAgent;
    const std::vector<std::string>& myTaskType;

    std::vector<std::vector<double>> myMxCost;  // cost matrix
    std::vector<int> myRowAgent;                // agent index in each row
    std::vector<int> myColTask;                 // task type index in each column

    double maxZero;

    void rowSubtract();         // subtract min row value from ech column
    void colSubtract();         // subtract min col value from each row
    bool isSolvable();          // check for a single zero in each row and column
    bool isFinished() const;    // check for all asignments completed

    /// @brief Assign agent to a task, remove agent and task from cost matrix
    /// @return string pair, agent name and task type

    std::pair<std::string, std::string> AssignReduce(); 

    int unAssignedAgentCount() const
    {
        return myMxCost.size();
    }
    int unAssignedTaskCount() const
    {
        if( ! unAssignedAgentCount() )
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

/// @brief Assign agents to task in multiple independant slots

class cAllocator
{
    std::vector<cAgent> myAgents;        // agents
    std::vector<std::string> myTaskType; // task type names
    std::vector<cTask> myTask;           // defined tasks
    std::vector<cSlot> mySlot;           // slots containg tasks

    solution_t mySolutionMaxflow;
    solution_t mySolutionHungarian;
    solution_t mySolutionAgents2Task;

    int mySlotCurrent;

    /// @brief find task type by name
    /// @param name
    /// @return task type index
    ///
    /// If task type absent, add it.

    int findTaskType(const std::string &name);

    bool isAgent(
        const std::string &name,
        int &iAgent) const;
    bool isSlot(const std::string &name);

    std::vector<int>
    taskTypeIndices(
        const std::string &vTask);

    std::vector<int> findAgentsForTask(int task);

    void writeFileSolution(
        std::ofstream& ofs,
        const char cid,
        const solution_t& solution ) const;

public:

    cAllocator()
    : mySlotCurrent( 0 )
    {}

    void clear();

    void addAgent(
        const std::string &name,
        const std::string &canDoTaskTypes,
        double cost = 1);

    void addTaskType(
        const std::string &stype);

    void addSlot(
        const std::string &name,
        const std::string &vTask);

    void example1();

    void readfile(const std::string &fname);
    void writefile(
        const std::string &fname);

    void setSlotFirst()
    {
        mySlotCurrent = 0;
    }
    void setSlotPrev()
    {
        mySlotCurrent--;
        if( mySlotCurrent < 0         )
            mySlotCurrent = 0;
    }
    void setSlotNext()
    {
        mySlotCurrent++;
        if( mySlotCurrent > mySlot.size()-1)
            mySlotCurrent = mySlot.size()-1;
    }
    void setSlotLast()
    {
        mySlotCurrent = mySlot.size()-1;
    }

    /// @brief Allocate agents to tasks using max flow algorithm

    void maxflow();

    /// @brief  Allocate agents to tasks using Hungarian algorithm
    /// https://en.wikipedia.org/wiki/Hungarian_algorithm

    void hungarian();

/*
<pre>
 - LOOP T over unassigned tasks
         - SET bestcost to MAX
         - SET bestagent to null
         - LOOP A over unassigned agents
             - IF A cannot be assigned to T
                 - CONTINUE
             - IF cost of assigning A to T < bestcost
                 - SET bestcost = cost of assigning A to T
                 - SET bestAgent = A
         - ENDLOOP A
         - ASSIGN bestAgent to T
         - IF no unassigned agents
             - BREAK
         - IF no unassigned tasks
            - BREAK
 - ENDLOOP T
 </pre>
*/
    void agents2tasks();

    const std::vector<cAgent>& getAgents() const
    {
        return myAgents;
    }
    const std::vector<std::string>& getTaskTypeNames() const
    {
        return myTaskType;
    }

    /// @brief getTaskTypeName
    /// @param i index of task
    /// @return type name

    std::string getTaskTypeName(int i) const
    {
        return myTaskType[myTask[i].myTaskType];
    }

    std::string getTaskTypeNameFromTypeID(int i) const
    {
        return myTaskType[i];
    }
    int getTaskTypeID(int i) const
    {
        return myTask[i].myTaskType;
    }

    std::string textProblem() const;
    std::string maxflowText() const
    {
        return textSolution(mySolutionMaxflow);
    }
    std::string hungarianText() const
    {
        return textSolution(mySolutionHungarian);
    }
    std::string agents2TasksText() const
    {
        return textSolution(mySolutionAgents2Task);
    }
    std::string textSolution(
        const solution_t &solution) const;

    std::string slotName() const
    {
        return mySlot[mySlotCurrent].name();
    }
};

bool unitTest();