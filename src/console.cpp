#include <iostream>
#include <fstream>
#include "allocator.h"

void errorHandler(
    const std::string &msg)
{
    int status = atoi(msg.c_str());
    if( ! status)
        status = 2;
    std::cout << msg << "\n" << "status " << status << "\n";
    exit(status);
}

main(int argc, char *argv[])
{
    if (argc != 3)
        errorHandler(
            "3 "
            "Usage: Agents2TasksConsole (input file path) ( output file path )");

    try
    {
        cAllocator A;
        readfile(A, argv[1]);
        A.maxflow();
        A.hungarian();
        Janusz(A);
        writefile(A, argv[2]);
    }
    catch (std::exception &e)
    {
        errorHandler(
            e.what());
    }

    return 0;
}