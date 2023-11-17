#include <iostream>
#include <fstream>
#include "allocator.h"

void errorHandler(
    const std::string &msg)
{
    int status = atoi(msg.c_str());
    if (!status)
        status = 2;
    std::cout << msg << "\n"
              << "status " << status << "\n";
    exit(status);
}

main(int argc, char *argv[])
{
    if (argc != 3)
        errorHandler(
            "3 "
            "Usage: Agents2TasksConsole (input file path) ( output file path )");

    cAllocator A;

    // run unit tests

    try
    {
        if (!unitTest())
            throw std::runtime_error("16 Unit test failed");
    }
    catch (std::exception &e)
    {
        std::string msg = "16 " + std::string(e.what());
        A.log(msg);
        throw std::runtime_error(msg);
    }
    A.log("Unit tests passed\n\n");

    // run calculation

    try
    {

        readfile(A, argv[1]);
        Janusz(A);
        writefile(A, argv[2]);
        A.log("\n============ Assignments ===========\n\n");
        A.log(A.getSolutionAgents2Task().textFile('A'));
    }
    catch (std::exception &e)
    {
        A.log("Exception " + std::string(e.what()));
        errorHandler(
            e.what());
    }

    return 0;
}