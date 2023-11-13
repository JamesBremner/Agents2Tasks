#include "allocator.h"

void Janusz(cAllocator &A)
{
    A.clearSolution();

    for (auto &slot : A.getSlots())
    {
        slotsolution_t slotSolution;
        int agentsUnassignedCount = A.getAgents().size();
        int tasksUnassignedCount = slot.taskCount();

        // unassign all agents
        A.unassignAgents();

        /* Sort agents in ascending order of previous assignments

          So that if there are multiple agents of equal cost available to do a task,
          the task will be assigned to the agent with the fewest assignments in the previous timeslots.

          So the uneven distribution of work to agents when there are more agents than work
          and the agents have the same cost per assignment
          will decrease as a percentage of total assignments
          as the number of timeslots increases
          with a limit of +/- 1 assignments per agent.

          https://github.com/JamesBremner/Agents2Tasks/issues/5#issuecomment-1801948876

        */

        A.sortAgentsByAssignedCount();

        /* Sort slot tasks into ascending order of number of agents capable of doing each task

        So that agents with rare capabilities are not 'wasted' by being assigned to tasks
        that could have been done by many other agents

        https://github.com/JamesBremner/Agents2Tasks/issues/13

        */

        auto sortedTaskIndex = A.sortTasksByAgentCount( slot );

        // loop over slot tasks
        for (int taskIndex : sortedTaskIndex)
        {
            cTask &task = A.task(taskIndex);

            // find cheapest unassigned agent that can do the task
            double bestCost = INT_MAX;
            cAgent *pbestAgent = 0;
            for (auto &agent : A.getAgents())
            {
                // std::cout << "try " << agent.name() << " for " << task.typeName() << "\n";

                // already assigned in this slot
                if (agent.isAssigned())
                    continue;

                // can't do the task
                if (!agent.isAble(task.taskType()))
                    continue;

                // assigned to task type recently
                if (agent.isAssignedRecently(
                        slot.day(),
                        task.typeName()))
                    continue;

                // is cheapest so far?
                double cost = agent.cost();
                if (cost < bestCost)
                {
                    bestCost = cost;
                    pbestAgent = &agent;
                }
            }
            if (!pbestAgent)
                continue;

            // assign cheapest agent

            // std::cout << "assigning " << pbestAgent->name() << " to " << task.typeName() << "\n";

            // add agent name, task type name pair to slot solution
            slotSolution.push_back(
                std::make_pair(
                    pbestAgent->name(),
                    task.typeName()));

            // mark task as assigned
            task.assign();
            tasksUnassignedCount--;

            // mark agent as assigned
            pbestAgent->assign(
                slot.day(),
                task.typeName());
            agentsUnassignedCount--;

            if (tasksUnassignedCount)
                std::cout << "Slot " << slot.name() << " unassigned tasks\n";

            // check for all agents assigned
            if (!agentsUnassignedCount)
                break;

        } // end of task loop

        A.add(
            slotSolution,
            slot.name(),
            A.slotCost(slotSolution));

    }   // end of slot loop
}
