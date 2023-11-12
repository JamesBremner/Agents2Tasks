#include <iostream>
#include <fstream>
#include "allocator.h"

void errorHandler(
    const std::string &fname,
    const std::string &msg)
{
    std::ofstream ofs(fname);
    if (!ofs.is_open())
        std::cout << "Cannot open output file\n";
    ofs << msg << "\n";
    ofs.close();
    std::cout << msg << "\n";
    exit(2);
}

main(int argc, char *argv[])
{
    if (argc != 3)
        errorHandler(
            "Agents2TasksConsole.txt",
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
            argv[2],
            e.what());
    }

    return 0;
}