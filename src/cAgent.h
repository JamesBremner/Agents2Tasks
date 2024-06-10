
#pragma once

class cAgent
{
protected:

    std::string myName;

    /* The tasks that this agent can be assigned to */
    std::vector<std::pair<std::string, double>> myTasks;

    int myFamily;                            // family index

    /* True if agent has been assigned to current timeslot */
    bool myAssigned;
    bool myfGroup;

    timepoint_t myLastAssignmentTime; //  00:00:01 on day of previous assignment

    /* The number of times agent assigned to a task in previous timeslots */
    int myAssignedCount;

    void parseTasks( int first, const std::vector<std::string> &vtoken );

public:

    cAgent() {}

    /// CTOR
    /// @param vtoken tokenized specification line

    cAgent(const std::vector<std::string> &vtoken);

    virtual void assign(int day);

    void unAssign()
    {
        myAssigned = false;
    }

    virtual bool isAssignedRecently(
        int day) const;

    void setPreviousTasks(int c)
    {
        myAssignedCount = c;
    }

    std::string name() const
    {
        return myName;
    }

    std::string user_name() const
    {
        if( ! isGroup() )
            return myName;
        return myName.substr(0,myName.length()-6);
    }

    double cost() const
    {
        if( ! myTasks.size() )
            return 0;
        return myTasks[0].second;
    }

    int family() const
    {
        return myFamily;
    }

    bool cando( const std::string& type ) const;

    bool isAssigned() const
    {
        return myAssigned;
    }

    int assignedCount() const
    {
        return myAssignedCount;
    }

    virtual std::vector<cAgent*> getMembers() const
    {
        std::vector<cAgent*>  ret;
        return ret;
    }

    bool isGroup() const
    {
        return myfGroup;
    }

    virtual std::string text() const;
    virtual std::string specText() const
    {
        return text();
    }


    static void clear();

    static void saveInputOrder();
    static void restoreInputOrder();

    /// @brief Add an agent
    /// @param vtoken tokenized specification line

    static void add(const std::vector<std::string> &vtoken);

    static bool isSane();

    static void unassignAll();

    static cAgent* find( const std::string& name );

    /// Sort agents into priority order
    static void sort(const cSlot *slot);
    static void sortAssignedCount();
    static void sortFamily(const cSlot *slot);

    /// get all the agents in sorted order

    static std::vector<cAgent *>
    get();


    /// get agents that can do task

    static std::vector<cAgent *>
    getForTask( const cTask& task );

    static std::string specTextAll();


};

class cAgentGroup : public cAgent
{
    std::vector<cAgent *> myMember;

public:
    cAgentGroup(const std::vector<std::string> &vtoken);

    virtual bool isAssignedRecently( int day ) const;

    virtual std::vector<cAgent*> getMembers() const
    {
        return myMember;
    }

    virtual std::string text() const;

    virtual std::string specText() const;

    static void add(const std::vector<std::string> &vtoken);

};
