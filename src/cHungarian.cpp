#include <stdexcept>
#include <iostream>

#include "allocator.h"

cHungarian::cHungarian(
    cAllocator &allocator,
    cSlot &slot)
    : myAgent(allocator.getAgents()),
      myTaskType(cTask::getTypeNames()),
      maxZero(0.00001)
{
    const double unablePenalty = 1000;

    for (int taskid : slot)
    {
        myColTask.push_back(allocator.getTaskTypeID(taskid));
    }

    // loop over the agents
    int iag = 0;
    for (auto &agent : allocator.getAgents())
    {
        myRowAgent.push_back(iag++);

        // loop over tasks required by slot
        std::vector<double> row;
        for (int task : slot)
        {
            // check if agent can be assigned to task
            if (!agent.isAble(allocator.getTaskTypeID(task)))
                row.push_back(unablePenalty);
            else
                row.push_back(agent.cost());
        }
        myMxCost.push_back(row);
    }
    colSubtract();
}

slotsolution_t cHungarian::assignAll()
{
    slotsolution_t ret;

    // loop until no more agents or no more task to be assigned
    while (!isFinished())
        ret.push_back(AssignReduce());

    return ret;
}

void cHungarian::rowSubtract()
{
    // loop over rows in cost matrix
    for (auto &row : myMxCost)
    {
        // determine minimum cost in row
        double min = INT_MAX;
        for (double c : row)
        {
            if (c < min)
            {
                min = c;
            }
        }
        // subtract min cost from each column in row
        for (double &c : row)
            c -= min;
    }
}

void cHungarian::colSubtract()
{
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        double min = INT_MAX;
        for (auto &row : myMxCost)
        {
            double c = row[col];
            if (c < min)
                min = c;
        }
        for (auto &row : myMxCost)
            row[col] -= min;
    }
}

bool cHungarian::isSolvable()
{
    if (myMxCost.size() == 1)
        return false;

    // count zeroes in rows
    for (auto &row : myMxCost)
    {
        int rowZeroCount = 0;
        for (double c : row)
        {
            if (c < maxZero)
            {
                rowZeroCount++;

                // check if more than one zero
                if (rowZeroCount > 1)
                    return false;
            }
        }
    }

    // count zeroes in cols
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        int colZeroCount = 0;
        for (int row = 0; row < myMxCost.size(); row++)
        {
            if (myMxCost[row][col] < maxZero)
            {
                colZeroCount++;

                // check if more than one
                if (colZeroCount > 1)
                    return false;
            }
        }
    }
    return true;
}

bool cHungarian::isFinished() const
{
    if ((!unAssignedTaskCount()) || (!unAssignedAgentCount()))
        return true;
    return false;
}

std::pair<std::string, std::string> cHungarian::AssignReduce()
{
    std::pair<std::string, std::string> retAgentTask;

    // check if just one agent left to be asigned
    if (unAssignedAgentCount() == 1)
    {
        // assign last agent to first available task
        // ( assumes fixed cost for agent on any task)
        retAgentTask.first = myAgent[myRowAgent[0]].name();
        retAgentTask.second = myTaskType[myColTask[0]];
        // std::cout << retAgentTask.first << " to "
        //           << retAgentTask.second << "\n";
        myMxCost.clear();
        return retAgentTask;
    }

    // check if just one task left to be assigned
    if (unAssignedTaskCount() == 1)
    {
        // assign cheapest unassigned agent to last task
        for (int row = 0; row < unAssignedAgentCount(); row++)
        {
            double bestCost = INT_MAX;
            int bestAgent = -1;
            for (int ai : myRowAgent)
            {
                double cost = myAgent[ai].cost();
                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestAgent = ai;
                }
            }

            retAgentTask.first = myAgent[bestAgent].name();
            retAgentTask.second = myTaskType[myColTask[0]];
            // std::cout << retAgentTask.first << " to "
            //           << retAgentTask.second << "\n";
            myMxCost.clear();
            return retAgentTask;
        }
    }

    // find 1st zero in column
    for (int row = 0; row < unAssignedAgentCount(); row++)
    {
        if (myMxCost[row][0] < maxZero)
        {
            // assign min cost agent to task
            retAgentTask.first = myAgent[myRowAgent[row]].name();
            retAgentTask.second = myTaskType[myColTask[0]];
            // std::cout << retAgentTask.first << " to "
            //           << retAgentTask.second << "\n";

            // check if multiple agents remain to be assigned
            if (myMxCost.size() > 1)
            {
                // remove assigned agent and task from cost matrix
                std::vector<std::vector<double>> tmp;
                for (int r = 0; r < myMxCost.size(); r++)
                {
                    if (r == row)
                        continue;
                    std::vector<double> vr;
                    for (int col = 1; col < myMxCost[0].size(); col++)
                        vr.push_back(myMxCost[row][0]);
                    tmp.push_back(vr);
                }
                myMxCost = tmp;

                myRowAgent.erase(myRowAgent.begin() + row);
                myColTask.erase(myColTask.begin());

                colSubtract();
            }

            return retAgentTask;
        }
    }
    throw std::runtime_error(
        "cHungarian::AssignReduce failed");
}
