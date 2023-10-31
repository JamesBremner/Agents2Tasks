#pragma once
#include <string>
#include <vector>

#include "../../PathFinder/src/GraphTheory.h"

typedef std::vector<raven::graph::cGraph> solution_t;

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
    cTask(int iType, int i)
        : myTaskType(iType),
          myIndex(i)
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

    std::vector<int>::iterator begin()
    {
        return myTasks.begin();
    }
    std::vector<int>::iterator end()
    {
        return myTasks.end();
    }
};

class cAllocator
{
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

    /// @brief Allocate agents to tasks using max flow algorithm
    void allocateMaxFlow();

    /// @brief  Allocate agents to tasks using Hungarian algorithm
    /// https://en.wikipedia.org/wiki/Hungarian_algorithm

    void allocateHungarian();

    std::string textProblem() const;
    std::string textMaxflow() const
    {
        return textSolution(mySolutionMaxflow);
    }
    std::string textHungarian() const
    {
        return textSolution(mySolutionHungarian);
    }
    std::string textSolution(
        const solution_t &solution) const;
};