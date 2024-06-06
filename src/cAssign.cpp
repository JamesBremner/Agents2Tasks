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
        for (cTask task : slot->getTasks())
        {
            if (task.isAssigned())
            {
                if (fexplain)
                    std::cout << "\ntask " << task.type()
                              << " in slot " << slot->name()
                              << ", already assigned";
                continue;
            }

            if (fexplain)
                std::cout << "\nAssigning task " << task.type()
                          << " in slot " << slot->name()
                          << ", available:";

            /* sort agents into priority order
            e.g. least workload or same family
            */

            cAgent::sort( slot );

            cAgent *pBestAgent = 0;

            // loop over agents 
            for (cAgent *pa : theDataStore.theAgents)
            {
                if( ! pa->cando( task.type()))
                    continue;

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
                if (pa->cost() < pBestAgent->cost())
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

                cAssign::add(
                    slot,
                    pBestAgent,
                    cTaskType::find(task.type()));
            }
        }
    }
}