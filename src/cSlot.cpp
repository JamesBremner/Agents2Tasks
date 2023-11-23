#include "assigner.h"
#include "cTask.h"
#include "cAgent.h"
#include "cAssign.h"
#include "cSlot.h"

std::vector<cSlot *> cSlot::theSlots;

cSlot::cSlot(const std::vector<std::string> &vtoken)
    : myName(vtoken[1])
{
    for (int k = 2; k < vtoken.size(); k++) {
        cTask* pt = cTask::find(vtoken[k]);
        if( !pt ) {
            pt = cTask::add(vtoken[k]);
        }
        myTasks.push_back(pt);
    }
}

bool cSlot::hasFamily( int family ) const
{
    return (myFamily.find(family) != myFamily.end());
}

std::string cSlot::text() const
{
    std::stringstream ss;

    ss
        << "t " << myName;
     for ( cTask* pt : myTasks)
        ss << " " <<  pt->name();

    return ss.str();
}