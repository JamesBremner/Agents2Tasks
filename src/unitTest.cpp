#include "allocator.h"

bool unitTest()
{
    cAllocator allocator;

    allocator.clear();
    allocator.addTaskType("teacher");
    allocator.addTaskType("cleaner");
    allocator.addTaskType("accountant");
    allocator.addAgent(
        "John",
        {"teacher cleaner"},
        3.0);
    allocator.addAgent(
        "Margaret",
        {"accountant cleaner"},
        4.0);
    allocator.addAgent(
        "Andrew",
        {"accountant teacher"},
        5.0);
    allocator.addSlot(
        "28/OCT/2023/8:30",
        {"teacher teacher cleaner"});

    allocator.hungarian();

    auto result = allocator.hungarianText();

    std::string expected =
        "John does teacher\nMargaret does teacher\nAndrew does cleaner\n\nCost 12\n";

    if (expected != result)
        return false;

    allocator.clear();
    allocator.addTaskType("teacher");
    allocator.addTaskType("cleaner");
    allocator.addTaskType("accountant");

    // add most expensive agent first
    // to test that the Hungarian optimizes the total slot cost
    
    allocator.addAgent(
        "Andrew",
        {"accountant teacher"},
        5.0);

    allocator.addAgent(
        "John",
        {"teacher cleaner"},
        3.0);
    allocator.addAgent(
        "Margaret",
        {"accountant cleaner"},
        4.0);
    allocator.addSlot(
        "2/NOV/2023/8:30",
        {"teacher accountant"});

    allocator.hungarian();
    result = allocator.hungarianText();

    expected =
        "John does teacher\nMargaret does accountant\n\nCost 7\n";
    if (expected != result)
        return false;

    allocator.clear();
    allocator.addTaskType("teacher");
    allocator.addTaskType("cleaner");
    allocator.addTaskType("accountant");

    // add most expensive agent first
    // to test that the Hungarian optimizes the total slot cost
    
    allocator.addAgent(
        "Andrew",
        {"accountant teacher"},
        5.0);

    allocator.addAgent(
        "John",
        {"teacher cleaner"},
        3.0);
    allocator.addAgent(
        "Margaret",
        {"accountant cleaner"},
        4.0);
    allocator.addSlot(
        "2/NOV/2023/8:30",
        {"teacher accountant"});

    Janusz(allocator);
    result = allocator.agents2TasksText();

    expected =
        "John does teacher\nMargaret does accountant\n\nCost 7\n";
    if (expected != result)
        return false;

    allocator.clear();
    allocator.addTaskType("teacher");
    
    allocator.addAgent(
        "Andrew",
        {"teacher"},
        1.0);
    allocator.addAgent(
        "John",
        {"teacher cleaner"},
        1.0);

    allocator.addSlot(
        "2/NOV/2023/8:30",
        {"teacher"});
    allocator.addSlot(
        "3/NOV/2023/8:30",
        {"teacher"});
    allocator.addSlot(
        "4/NOV/2023/8:30",
        {"teacher"});

    Janusz(allocator);

    allocator.setSlotFirst();
    result = allocator.agents2TasksText();
    expected =
        "Andrew does teacher\n\nCost 1\n";
    if (expected != result)
        return false;
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    expected =
        "John does teacher\n\nCost 1\n";
    if (expected != result)
        return false;

// test https://github.com/JamesBremner/Agents2Tasks/issues/10

    allocator.clear();
    allocator.addTaskType("teacher");
    
    allocator.addAgent(
        "Andrew",
        {"teacher"},
        1.0);


    allocator.addSlot(
        "202310311000",
        {"teacher"});
    allocator.addSlot(
        "202311011300",
        {"teacher"});

    Janusz(allocator);

    allocator.setSlotFirst();
    result = allocator.agents2TasksText();
    expected =
        "Andrew does teacher\n\nCost 1\n";
    if (expected != result)
        return false;
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    expected =
        "\nCost 0\n";
    if (expected != result)
        return false;

    return true;
}