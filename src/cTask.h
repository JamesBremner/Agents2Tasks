#pragma once

// a particular task in a timeslot
class cTask
{
    std::string myType;
    bool myfAssigned;

public:
    cTask(const std::string &t)
        : myType(t), myfAssigned(false)
    {
    }

    void assign()
    {
        myfAssigned = true;
    }
    bool isAssigned() const
    {
        return myfAssigned;
    }
    std::string type() const
    {
        return myType;
    }
};
