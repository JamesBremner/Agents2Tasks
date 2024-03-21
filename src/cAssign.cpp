#include "Agents2Tasks.h"

cAssign::cAssign(cSlot *ps, cAgent *pa, cTask *pt)
    : myAgent(pa),
      myTask(pt),
      mySlot(ps),
      myGroup(0)
{
    pa->assign(ps->day());
    ps->assign(pa->family());
}

cAssign::cAssign(cSlot *ps, cAgent *pa, cTask *pt, cAgentGroup *pg)
    : cAssign(ps, pa, pt)
{
    myGroup = pg;
}

void cAssign::clear()
{
    for (auto *pa : theDataStore.theAssigns)
        delete pa;
    theDataStore.theAssigns.clear();
}

std::vector<cAssign *>
cAssign::getSlotAssigns(cSlot *slot)
{
    std::vector<cAssign *> ret;
    for (auto *a : theDataStore.theAssigns)
        if (a->mySlot == slot)
            ret.push_back(a);
    return ret;
}

void cAssign::add(cSlot *ps, cAgent *pa, cTask *pt)
{
    if (!(ps && pa && pt))
        return;

    theDataStore.theAssigns.push_back(
        new cAssign(ps, pa, pt));
    if (pa->name().find("_group") != -1)
    {
        // assign group agent  members
        for (cAgent *pm : pa->getMembers())
            theDataStore.theAssigns.push_back(
                new cAssign(ps, pm, pt, (cAgentGroup *)pa));
    }
}

std::string cAssign::text(const std::string &slotName) const
{
    std::stringstream ss;

    ss << "A " << slotName
       << " " << myAgent->name();
    if (myGroup)
        ss << " in " << myGroup->user_name();
    ss << " to " << myTask->name();
    return ss.str();
}

std::string cAssign::text(cSlot *pSlot)
{
    std::string ret;
    for (auto *pAssign : theDataStore.theAssigns)
        if (pAssign->slot() == pSlot)
            ret += pAssign->text(pSlot->name()) + "\n";
    return ret;
}

void Agents2Tasks(bool fexplain)
{
    cAssign::clear();

    // loop over slots
    for (cSlot *slot : cSlot::get())
    {
        // remove any assignments from a previous timeslot

        cAgent::unassignAll();

        // remove any changes in agent order from a previous timeslot
        // TID35

        cAgent::restoreInputOrder();

        // loop over tasks required by slot
        for (cTask *ptask : slot->getTasks())
        {
            if (fexplain)
                std::cout << "\nAssigning task " << ptask->name()
                          << " in slot " << slot->name()
                          << ", available:";

            /* sort agents
            by workload first then by family
            so family gets priority
            */

            // std::cout << "before sort\n";
            // for (cAgent *pa : cAgent::get())
            //     std::cout << pa->name() << "\n";

            cAgent::sortAssignedCount();
            cAgent::sortFamily(slot);

            // std::cout << "after sort\n";
            // for (cAgent *pa : cAgent::get())
            //     std::cout << pa->name() << "\n";

            cAgent *pBestAgent = 0;

            // loop over agents that can do the task
            for (cAgent *pa : cAgent::getForTask(ptask))
            {
                if (pa->isAssigned())
                    continue;

                if (pa->isAssignedRecently(slot->day()))
                    continue;

                if (fexplain)
                    std::cout << " " << pa->name()
                        << " w" << pa->assignedCount() << ", ";

                if (!pBestAgent)
                {
                    pBestAgent = pa;
                    continue;
                }

                // check for cheaper
                if (pa->cost(ptask) < pBestAgent->cost(ptask))
                    pBestAgent = pa;
            }

            if (!pBestAgent)
            {
                if (fexplain)
                    std::cout << "no available agents";
            }
            else
            {
                // assign best agent to task

                if (fexplain)
                    std::cout << " assigned: " << pBestAgent->name();

                cAssign::add(slot, pBestAgent, ptask);
            }
        }
    }
}