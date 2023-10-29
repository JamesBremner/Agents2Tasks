#pragma once
#include <string>
#include <vector>

class cAgent
{
    std::string myName;
    std::vector<int> myTasks;
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
};

class cAllocator
{
    std::vector<cAgent> myAgents;
    std::vector<cTask> myTasks;
    std::vector<std::string> myTaskType;
    std::vector<cSlot> mySlot;

public:
    void addAgent(
        const std::string &name,
        const std::vector<int> &canDoTaskTypes);

    void addTask(
        const std::string &stype);

    void addSlot(
        const std::string &name,
        const std::vector<std::string> &vTask);

    std::string text() const;
};