#pragma once

class cTask
{

    std::string myName;

    public:

    cTask(  const std::string& name )
    : myName( name )
    {}

    std::string name() const
    {
        return myName;
    }

    static void clear();

    static cTask* add( const std::string& name );

    static std::vector< cTask* >
    get();

    static cTask* find( const std::string& name );

};

