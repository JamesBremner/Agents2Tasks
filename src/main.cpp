#include "Agents2Tasks.h"

sDataStore theDataStore;

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

void parseCommand(
    int argc, char *argv[],
    bool &fexplain, std::string &infilename, std::string &outfilename)
{
    if (argc != 3 && argc != 4)
        errorHandler(
            "3 "
            "Usage: Agents2Tasks <explain> (input file path) ( output file path )");

    fexplain = false;
    infilename = argv[1];
    outfilename = argv[2];
    if (argc == 4)
    {
        fexplain = true;
        infilename = argv[2];
        outfilename = argv[3];
    }
}

main(int argc, char *argv[])
{
    bool fexplain;
    std::string infilename;
    std::string outfilename;
    parseCommand(
        argc, argv,
        fexplain, infilename, outfilename);

    // run unit tests

    try
    {
        if (!unitTest())
            throw std::runtime_error("16 Unit test failed");
    }
    catch (std::exception &e)
    {
        std::string msg = "16 " + std::string(e.what());
        // A.log(msg);
        throw std::runtime_error(msg);
    }
    std::cout << "Unit tests passed\n\n";

    // run calculation

    try
    {

        readfile(infilename);
        Agents2Tasks(fexplain);
        writefile(outfilename);

        // A.log("\n============ Assignments ===========\n\n");
        // A.log(A.getSolutionAgents2Task().textFile('A'));
    }
    catch (std::exception &e)
    {
        // A.log("Exception " + std::string(e.what()));
        errorHandler(
            e.what());
    }

    return 0;
}