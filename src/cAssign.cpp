#include "assigner.h"
#include "cTask.h"
#include "cAgent.h"
#include "cAssign.h"
#include "cSlot.h"

std::vector<cAssign *> cAssign::theAssigns;

cAssign::cAssign(cSlot *ps, cAgent *pa, cTask *pt)
    : myAgent(pa),
      myTask(pt),
      mySlot(ps)
{
    pa->assign(ps->day());
    ps->assign(pa->family());
}

std::string cAssign::text(cSlot *pSlot)
{
    std::string ret;
    for (auto *pAssign : theAssigns)
        if (pAssign->slot() == pSlot)
            ret += pAssign->text() + "\n";
    return ret;
}

std::vector<cAssign *>
cAssign::getSlotAssigns(cSlot *slot)
{
    std::vector<cAssign *> ret;
    for (auto *a : theAssigns)
        if (a->mySlot == slot)
            ret.push_back(a);
    return ret;
}

void cAssign::add(cSlot *ps, cAgent *pa, cTask *pt)
{
    theAssigns.push_back(
        new cAssign(ps, pa, pt));
}

void assign()
{
    cAssign::clear();

    // loop over slots
    for (cSlot *slot : cSlot::getAll())
    {
        cAgent::unassignAll();

        // loop over tasks required by slot
        for (cTask *pt : slot->getTasks())
        {

            /* sort agents
            by workload first then by family
            so family gets priority
            */
            cAgent::sortAssignedCount();
            cAgent::sortFamily(slot);

            // std::cout << "\n";
            // for( cAgent* a : cAgent::getAll() )
            //     std::cout << a->text();

            cAgent *pBestAgent = 0;

            // loop over agents
            for (cAgent *pa : cAgent::getAll())
            {
                if (pa->isAssigned())
                    continue;

                if( pa->isAssignedRecently( slot->day() ))
                    continue;

                if (!pBestAgent)
                {
                    pBestAgent = pa;
                    continue;
                }
                if (pa->cost(pt) < pBestAgent->cost(pt))
                    pBestAgent = pa;
            }

            // assign best agent to task
            if (pBestAgent)
                cAssign::add(slot, pBestAgent, pt);
        }
    }
}