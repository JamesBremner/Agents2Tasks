
// standard libraries

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>

typedef std::chrono::system_clock::time_point timepoint_t;

// Application classes

class cSlot;
#include "cTask.h"
#include "cAgent.h"
#include "cAssign.h"
#include "cSlot.h"

// Application data store

struct sDataStore
{
    std::vector<cTaskType *> theTasks;
    std::vector<cSlot *> theSlots;

    /* The agents in sorted order */
    std::vector<cAgent *> theAgents;

    /* The agents in input order */
    std::vector<cAgent *> theAgentsInputOrder;

    std::vector<std::string> vFamily; // family names

    /* The assignments */
    std::vector<cAssign *> theAssigns;
};

extern sDataStore theDataStore;

// free functions

void readfile(
    const std::string &fname);

void writefile(
    const std::string &fname);

void readstring(
    const std::string &sin);

bool unitTest();

std::string specText();
