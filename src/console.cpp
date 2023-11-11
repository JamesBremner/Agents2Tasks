#include <iostream>
#include "allocator.h"

main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: Agents2TasksConsole (input file path) ( output file path )";
        exit(1);
    }

    cAllocator A;
    A.readfile(argv[1]);
    A.maxflow();
    A.hungarian();
    A.agents2tasks();
    writefile( A, argv[2] );

    return 0;
}