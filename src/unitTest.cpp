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
        "202310280830",
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
        "202311020830",
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
         "202311020830",
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
         "202311020830",
        {"teacher"});
    allocator.addSlot(
         "202311030830",
        {"teacher"});
    allocator.addSlot(
         "202311040830",
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

    readstring( 
        allocator,
        "a Andrew 1 teacher\n"
        "a Bob 1 cleaner\n"
        "t 20231031100 teacher\n"
        "t 20231101130 teacher\n" );

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
        "No tasks assigned\n";
    if (expected != result)
        return false;

    readstring( 
        allocator,
        "a Alice 1 teacher cleaner\n"
        "a Bob 1 cleaner\n"
        "t 202311091000 cleaner teacher\n" );
    Janusz(allocator);
    allocator.setSlotFirst();
    result = allocator.agents2TasksText();


    return true;
}