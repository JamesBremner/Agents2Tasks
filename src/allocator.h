#pragma once
#include <string>
#include <vector>

class cAgent
{
    std::string myName;
    std::vector<int> myTasks;

public:
    cAgent(
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

    bool isAble(int task);

    std::string text(
        const std::vector<std::string> vTaskType) const;
};

class cTask
{
    int myType;
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
        const std::vector<std::string> vTaskType) const;

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
    std::vector<cAgent> myAgents;
    std::vector<cTask> myTasks;
    std::vector<std::string> myTaskType;
    std::vector<cSlot> mySlot;
    std::vector<std::vector<std::string>> mySolution;

    /// @brief find task type by name
    /// @param name
    /// @return task type index
    ///
    /// If task type absent, add it.

    int findTaskType(const std::string &name);
    bool isAgent(const std::string &name);
    bool isSlot(const std::string &name);


    std::vector<int>
    taskTypeIndices(
        const std::string &vTask);

    std::vector<int> findAgentsForTask(int task);

public:
    void clear();

    void addAgent(
        const std::string &name,
        const std::string &canDoTaskTypes);

    void addTask(
        const std::string &stype);

    void addSlot(
        const std::string &name,
        const std::string &vTask);

    void allocate();

    std::string text() const;
};