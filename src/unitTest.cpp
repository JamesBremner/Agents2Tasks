#include "assigner.h"
#include "cTask.h"
#include "cAgent.h"
#include "cAssign.h"
#include "cSlot.h"

bool unitTest()
{
    std::string utname, result, expected;

    utname = "1";
    readstring(
        "a Carol 1 A cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 C cleaner\n"
        "t 202311011000 cleaner cleaner\n");

    result = specText();

    expected =
        "a Carol 1 A cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 C cleaner\n"
        "t 202311011000 cleaner cleaner";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

    assign();

    result = cAssign::text(cSlot::getAll()[0]);
    expected =
        "Carol to cleaner\nBob to cleaner\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

/////////////////////////////////

    utname = "even workload";
    readstring(
        "a Carol 1 C cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 A cleaner\n"
        "t 202311011000 cleaner cleaner\n"
        "t 202311011200 cleaner\n");

    assign();

    // Alice gets job in 2nd timeslot because she has the least workload
    result = cAssign::text(cSlot::getAll()[1]);
    expected =
        "Alice to cleaner\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

/////////////////////////////////////////////

    utname = "prefer family";
    readstring(

        "a Carol 1 A cleaner\n"
        "a Bob 1 B cleaner\n"
        "a Alice 1 A cleaner\n"
        "t 202311011000 cleaner cleaner\n");

    // give Alice a history of heavy workload
    cAgent::getAll()[2]->setPreviousTasks( 10 );


    assign();
    result = cAssign::text(cSlot::getAll()[0]);
    
    // Carol and Alice gets the task because 
    // although Alice has a greater previous workload than Bob
    // the family group gets priority over workload
    // https://github.com/JamesBremner/Agents2Tasks/issues/21#issuecomment-1813504598
    
    expected = "Carol to cleaner\nAlice to cleaner\n";
    if (expected != result)
        throw std::runtime_error(utname + " unit test failed");

    return true;
}
