#include "Agents2Tasks.h"

cAssign::cAssign(cSlot *ps, cAgent *pa, cTaskType *pt)
    : myAgent(pa),
      myTask(pt),
      mySlot(ps),
      myGroup(0)
{
    pa->assign(ps->day());
    ps->assign(pa->family());
}

cAssign::cAssign(cSlot *ps, cAgent *pa, cTaskType *pt, cAgentGroup *pg)
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

void cAssign::add(cSlot *ps, cAgent *pa, cTaskType *pt)
{
    if (!(ps && pa && pt))
        return;

    if (!pa->isGroup())
    {
        // individual agent
        // assign it to the first unassigned task of type pt
        ps->firstUnassigned(pt).assign();
        theDataStore.theAssigns.push_back(
            new cAssign(ps, pa, pt));
    }
    else
    {
        // group agent

        theDataStore.theAssigns.push_back(
            new cAssign(ps, pa, pt));
        theDataStore.theAssigns.back()->set((cAgentGroup *)pa);

        // loop over group members
        for (cAgent *pma : ((cAgentGroup *)pa)->getMembers())
        {
            if (pma->isAssigned())
                continue;

            // loop over timeslot tasks
            for (int kt = 0; kt < ps->getTasks().size(); kt++)
            {
                if (ps->isTaskAssigned(kt))
                    continue;

                theDataStore.theAssigns.push_back(
                    new cAssign(ps, pma, pt, (cAgentGroup *)pa));
                ps->taskAssign(kt);
                pa->assign(ps->day());

                break;
            }
        }
    }
}

std::string cAssign::text(const std::string &slotName) const
{
    std::stringstream ss;

    ss << "A " << slotName << " ";
    if (myAgent->isGroup())
        ss << myAgent->name() << " assigned";
    else
    {
        ss << myAgent->name();
        if (myGroup)
            ss << " in " << myGroup->user_name();
        ss << " to " << myTask->name();
    }
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
        // for (cTask *ptask : slot->getTasks())
        for (int ktask = 0; ktask < slot->getTasks().size(); ktask++)
        {
            // cTaskType *ptask = slot->getTasks()[ktask];
            cTaskType *ptask = cTaskType::find(slot->getTasks()[ktask].type());

            if (slot->isTaskAssigned(ktask))
            {
                if (fexplain)
                    std::cout << "\ntask " << ptask->name()
                              << " in slot " << slot->name()
                              << ", already assigned";
                continue;
            }

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