#include "allocator.h"

bool unitTest()
{
    cAllocator allocator;

    std::string utname, result, expected;

    utname = "tid21-1";
    readstring(
        allocator,
        "a Carol 1 A cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 A cleaner\n"
        "t 202311011000 cleaner cleaner\n");
    Janusz(allocator);
    allocator.setSlotFirst();
    result = allocator.agents2TasksText();

    // alice gets the second clear job because, unlike Bob, she is in same group as Carol
    expected = "Carol does cleaner\nAlice does cleaner\n\nCost 2\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

    utname = "tid21-2";
    readstring(
        allocator,
        "a Carol 1 A cleaner\n"
        "a Alice 1 A cleaner\n"
        "t 202311011000 cleaner\n"
        "t 202312011000 cleaner\n");

    Janusz(allocator);
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    
    // alice gets the task in 2nd slot because she has the least previous workload
    expected = "Alice does cleaner\n\nCost 1\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotFirst();

    utname = "tid21-3";
    readstring(
        allocator,
        "a Carol 1 A cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 A cleaner\n"
        "t 202311011000 cleaner cleaner\n");

    // give Alice a history of heavy workload
    allocator.getAgents()[2].setPreviousTasks( 10 );

    Janusz(allocator);
    result = allocator.agents2TasksText();
    
    // alice gets the task because 
    // although she has a greater previous workload than Bob
    // the family group gets priority over workload
    // https://github.com/JamesBremner/Agents2Tasks/issues/21#issuecomment-1813504598
    
    expected = "Carol does cleaner\nAlice does cleaner\n\nCost 2\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotFirst();

    // check blocking, not per task type
    utname = "tid17";
    readstring(
        allocator,
        "a alice 1 G teacher cleaner\n"
        "t 202311010830 G cleaner\n"
        "t 202311020830 G teacher\n"
        "t 202311031000 G teacher\n");

    Janusz(allocator);

    allocator.setSlotFirst();
    result = allocator.agents2TasksText();
    expected =
        "alice does cleaner\n\nCost 1\n";
    if (expected != result)
         throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    expected =
        "No tasks assigned\n";
    if (expected != result)
         throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    expected =
        "alice does teacher\n\nCost 1\n";
    if (expected != result)
         throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotFirst();

    utname = "costOptimizing";

    // add most expensive agent first
    // to test that the Hungarian optimizes the total slot cost

    readstring(
        allocator,
        "a Andrew 5 A accountant teacher\n"
        "a John 3 A teacher cleaner\n"
        "a Margaret 4 A accountant cleaner\n"
        "t 202311020830 teacher accountant\n"    );

    Janusz(allocator);
    result = allocator.agents2TasksText();

    expected =
        "John does teacher\nMargaret does accountant\n\nCost 7\n";
    if (expected != result)
         throw std::runtime_error(utname + " unit test failed");

    utname = "tid13";
    readstring(
        allocator,
        "a Andrew 1 A teacher\n"
        "a John 1 A teacher cleaner\n"
        "t 202311020830 teacher\n"
        "t 202311030830 teacher\n"
        "t 202311040830 teacher\n"
    );

    Janusz(allocator);

    allocator.setSlotFirst();
    result = allocator.agents2TasksText();
    expected =
        "Andrew does teacher\n\nCost 1\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");
    allocator.setSlotNext();
    result = allocator.agents2TasksText();
    expected =
        "John does teacher\n\nCost 1\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

    // test https://github.com/JamesBremner/Agents2Tasks/issues/10
    utname = "tid10";
    readstring(
        allocator,
        "a Andrew 1 teacher\n"
        "a Bob 1 cleaner\n"
        "t 20231031100 teacher\n"
        "t 20231101130 teacher\n");

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

    return true;
}