#pragma once
#include <string>
#include <vector>

#include "../../PathFinder/src/GraphTheory.h"

typedef std::vector<std::pair<std::string,std::string>> slotsolution_t;
typedef std::vector<slotsolution_t> solution_t;

class cAllocator;

class cAgent
{
    std::string myName;

    /// @brief tasks agent ready to do ( task type id, cost )
    std::vector<std::pair<int, double>> myTasks;

public:
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

    std::string text(
        const std::vector<std::string> vTaskType) const;
};

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

class cSlot
{
    std::string myName;
    std::vector<int> myTasks;

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
        const std::vector<std::string> &vTaskType) const;

    int taskCount() const
    {
        return myTasks.size();
    }

    int taskID( int taskpos) const
    {
        return myTasks[ taskpos ];
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
};

class cHungarian
{
    double maxZero;
    std::vector<int> myAgent;   // agent index in each column
    std::vector<std::string> myTaskType;

        void rowSubtract();

public:
    std::vector<std::vector<double>> myMxCost;

    cHungarian( 
        cAllocator& allocator,
        cSlot& slot );

    bool isSolvable();
    bool isFinished() const;

    std::pair<int,std::string> AssignReduce();
};

class cAllocator
{
    //typedef std::vector<std::vector<double>> hungarianCostMatrix_t;

    std::vector<cAgent> myAgents;        // agents
    std::vector<std::string> myTaskType; // task type names
    std::vector<cTask> myTask;           // defined tasks
    std::vector<cSlot> mySlot;           // slots containg tasks

    solution_t mySolutionMaxflow;
    solution_t mySolutionHungarian;

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

    /// @brief  construct cost martix, each agent gets a row, each task gets a column
    /// @param slot
    /// @return

public:
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

    /// @brief Allocate agents to tasks using max flow algorithm
    
    void maxflow();

    /// @brief  Allocate agents to tasks using Hungarian algorithm
    /// https://en.wikipedia.org/wiki/Hungarian_algorithm

    void hungarian();

    std::vector<cAgent> getAgents() const
    {
        return myAgents;
    }
    std::string getTaskTypeName(int i ) const
    {
        return myTaskType[myTask[i].myTaskType];
    }
    int getTaskTypeID(int i ) const
    {
        return myTask[i].myTaskType;
    }

    std::string textProblem() const;
    std::string textMaxflow() const
    {
        return textSolution(mySolutionMaxflow);
    }
    std::string hungarianText() const
    {
        return textSolution(mySolutionHungarian);
    }
    std::string textSolution(
        const solution_t &solution) const;
};