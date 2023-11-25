#include "Agents2Tasks.h"


 void cTask::clear()
    {
        for( auto* pt : theDataStore.theTasks )
            delete pt;
        theDataStore.theTasks.clear();
    }

        static std::vector< cTask* >
    get()
    {
        return theDataStore.theTasks;
    }

cTask *cTask::find(const std::string &name)
{
    auto it = std::find_if(
        theDataStore.theTasks.begin(), theDataStore.theTasks.end(),
        [&](cTask *pt)
        {
            return (pt->name() == name);
        });
    if (it == theDataStore.theTasks.end())
        return 0;
    return *it;
}

cTask *cTask::add(const std::string &name)
{
    if (find(name))
        throw std::runtime_error("25 Duplicate task name");
    theDataStore.theTasks.push_back(
        new cTask(name));
    return theDataStore.theTasks.back();
}