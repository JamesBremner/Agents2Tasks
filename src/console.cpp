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
    readfile(A,argv[1]);
    A.maxflow();
    A.hungarian();
    Janusz(A);
    writefile( A, argv[2] );

    return 0;
}