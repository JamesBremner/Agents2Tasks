#include <fstream>
#include <sstream>
#include "allocator.h"

void readfile(
    cAllocator &allocator,
    const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("15 "
            "Cannot open input file");
    input(allocator, ifs);
}
void readstring(
    cAllocator &allocator,
    const std::string &sin)
{
    std::stringstream ins(sin);
    input(allocator, ins);
}
void input(
    cAllocator &allocator,
    std::istream &ifs)
{
    allocator.clear();
    std::string line;
    while (getline(ifs, line))
    {
        std::vector<std::string> vtoken;
        std::stringstream sst(line);
        std::string a;
        while (getline(sst, a, ' '))
            vtoken.push_back(a);
        int p;

        switch (line[0])
        {
        case 'a':
            p = line.find(" ");
            p = line.find(" ", p + 1);
            p = line.find(" ", p + 1);
            p = line.find(" ", p + 1);
            allocator.addAgent(
                vtoken[1],
                line.substr(p + 1),
                atof(vtoken[2].c_str()),
                vtoken[3]);
            break;

        case 't':
            p = line.find(" ");
            p = line.find(" ", p + 1);
            allocator.addSlot(
                vtoken[1],
                line.substr(p + 1));
            break;
        }
    }
    allocator.isSlotSane();
}
