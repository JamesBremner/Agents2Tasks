#include <stdexcept>

#include "allocator.h"

cHungarian::cHungarian(
    cAllocator &allocator,
    cSlot &slot)
    : myAgent(allocator.getAgents()),
      maxZero(0.00001)
{
    const double unablePenalty = 1000;

    for (int taskid : slot)
    {
        myColTaskType.push_back(allocator.getTaskTypeName(taskid));
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
    rowSubtract();
}

slotsolution_t cHungarian::assignAll()
{
    slotsolution_t ret;

    // loop until no more agents or no more task to be assigned
    while (!isFinished())
        ret.push_back( AssignReduce() );

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
        // sebtract min cost from each column in row
        for (double &c : row)
            c -= min;
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
    if (!myMxCost.size())
        return true;              // all agents assigned
    return (!myMxCost[0].size()); // check for all task assigned
}

std::pair<std::string, std::string> cHungarian::AssignReduce()
{
    std::pair<std::string, std::string> retAgentTask;

    // check if just one agent left to be asigned
    if (myMxCost.size() == 1)
    {
        // assign last task to last agent
        retAgentTask.first = myAgent[myRowAgent[0]].name();
        retAgentTask.second = myColTaskType[0];
        myMxCost.clear();
        return retAgentTask;
    }

    // find 1st zero in column
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        if (myMxCost[0][col] < maxZero)
        {
            // assign min cost agent to task
            retAgentTask.first =  myAgent[myRowAgent[0]].name();
            retAgentTask.second = myColTaskType[col];

            // check if agents remain to be assigned
            if (myMxCost.size() > 1)
            {
                // remove assigned agent and task from cost matrix
                std::vector<std::vector<double>> tmp;
                for (int row = 1; row < myMxCost.size(); row++)
                {
                    std::vector<double> vr;
                    for (int c = 0; c < myMxCost[0].size(); c++)
                    {
                        if (c != col)
                            vr.push_back(myMxCost[row][c]);
                    }
                    tmp.push_back(vr);
                }
                myMxCost = tmp;

                myRowAgent.erase(myRowAgent.begin());
                myColTaskType.erase(myColTaskType.begin() + col);
            }

            return retAgentTask;
        }
    }
    throw std::runtime_error(
        "cHungarian::AssignReduce failed");
}
