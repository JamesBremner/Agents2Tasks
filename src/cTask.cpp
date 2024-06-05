#include "Agents2Tasks.h"


 void cTaskType::clear()
    {
        for( auto* pt : theDataStore.theTasks )
            delete pt;
        theDataStore.theTasks.clear();
    }

        static std::vector< cTaskType* >
    get()
    {
        return theDataStore.theTasks;
    }

cTaskType *cTaskType::find(const std::string &name)
{
    auto it = std::find_if(
        theDataStore.theTasks.begin(), theDataStore.theTasks.end(),
        [&](cTaskType *pt)
        {
            return (pt->name() == name);
        });
    if (it == theDataStore.theTasks.end())
        return 0;
    return *it;
}

cTaskType *cTaskType::add(const std::string &name)
{
    if (find(name))
        throw std::runtime_error("25 Duplicate task name");
    theDataStore.theTasks.push_back(
        new cTaskType(name));
    return theDataStore.theTasks.back();
}