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

    int iag = 0;
    for (auto &agent : allocator.getAgents())
    {
        myRowAgent.push_back(iag++);

        std::vector<double> row;
        double delta = 0;
        for (int task : slot)
        {
            if (!agent.isAble(allocator.getTaskTypeID(task)))
                row.push_back(unablePenalty);
            else
            {
                row.push_back(agent.cost());

                // we have assumed that each agent costs the same
                // no matter which task they do
                // The Hungarian algorithm needs a different cost for each task
                delta += 0.01;
            }
        }
        myMxCost.push_back(row);
    }
    rowSubtract();
}

slotsolution_t cHungarian::assignAll()
{
    slotsolution_t ret;

    while (!isFinished())
        ret.push_back( AssignReduce() );

    return ret;
}

void cHungarian::rowSubtract()
{
    for (auto &row : myMxCost)
    {
        double min = INT_MAX;
        for (double c : row)
        {
            if (c < min)
            {
                min = c;
            }
        }
        for (double &c : row)
            c -= min;
    }
}

bool cHungarian::isSolvable()
{
    if (myMxCost.size() == 1)
        return false;

    for (auto &row : myMxCost)
    {
        int rowZeroCount = 0;
        for (double c : row)
        {
            if (c < maxZero)
            {
                rowZeroCount++;
                if (rowZeroCount > 1)
                    return false;
            }
        }
    }
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        int colZeroCount = 0;
        for (int row = 0; row < myMxCost.size(); row++)
        {
            if (myMxCost[row][col] < maxZero)
            {
                colZeroCount++;
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
    if (myMxCost.size() == 1)
    {
        // assign last task to last agent
        retAgentTask.first = myAgent[myRowAgent[0]].name();
        retAgentTask.second = myColTaskType[0];
        myMxCost.clear();
        return retAgentTask;
    }
    for (int col = 0; col < myMxCost[0].size(); col++)
    {
        if (myMxCost[0][col] < maxZero)
        {
            retAgentTask.first =  myAgent[myRowAgent[0]].name();
            retAgentTask.second = myColTaskType[col];

            if (myMxCost.size() > 1)
            {
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
