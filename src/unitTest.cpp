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
        "John does teacher\nMargaret does cleaner\nAndrew does teacher\n\nCost 12\n";

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

    return true;
}