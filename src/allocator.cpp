#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "allocator.h"

std::string cSlot::text(
    const std::vector<std::string> vTaskType) const
{
    std::stringstream ss;
    ss << myName << " tasks: ";
    for (int tsk : myTasks)
        ss << vTaskType[tsk] << " ";
    return ss.str();
}

void cAllocator::addAgent(
    const std::string &name,
    const std::vector<int> &canDoTaskTypes)
{
}

void cAllocator::addTask(
    const std::string &stype)
{
    if (std::find(
            myTaskType.begin(), myTaskType.end(), stype) != myTaskType.end())
        return;
    myTaskType.push_back(stype);
}

void cAllocator::addSlot(
    const std::string &name,
    const std::vector<std::string> &vTask)
{
    if (std::find_if(
            mySlot.begin(), mySlot.end(),
            [name](const cSlot &sl)
            {
                return (sl.name() == name);
            }) != mySlot.end())
        throw std::runtime_error(
            "Duplicate slot name");

    std::vector<int> vTaskIndices;
    for (int kt = 0; kt < vTask.size(); kt++)
    {
        int task;
        auto it = std::find(
            myTaskType.begin(), myTaskType.end(), vTask[kt]);
        if (it == myTaskType.end())
        {
            myTaskType.push_back(vTask[kt]);
            vTaskIndices.push_back(myTaskType.size() - 1);
        }
        else
        {
            vTaskIndices.push_back(it - myTaskType.begin());
        }
    }
    mySlot.emplace_back(name, vTaskIndices);
}

std::string cAllocator::text() const
{
    std::stringstream ss;

    for (auto &slot : mySlot)
    {
        ss << slot.text(myTaskType) << "\n\n";
    }

    return ss.str();
}