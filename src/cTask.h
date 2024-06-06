#pragma once

/// a generic task type

class cTaskType
{

    std::string myName;

    public:

    cTaskType(  const std::string& name )
    : myName( name )
    {}

    std::string name() const
    {
        return myName;
    }

    static void clear();

    static cTaskType* add( const std::string& name );

    static std::vector< cTaskType* >
    get();

    static cTaskType* find( const std::string& name );

};

// a particular task in a timeslot
class cTask
{
    cTaskType * myType;
    bool myfAssigned;

    public:
    cTask( cTaskType * t)
    : myType( t )
    , myfAssigned( false )
    {}

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
        return myType->name();
    }
};

