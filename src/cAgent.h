
#pragma once

class cAgent
{
protected:

    /* The agents in sorted order */
    static std::vector<cAgent *> theAgents;

    /* The agents in input order */
    static std::vector<cAgent *> theAgentsInputOrder;

    std::string myName;

    /* The tasks that this agent can be assigned to */
    std::vector<std::pair<cTask *, double>> myTasks;

    static std::vector<std::string> vFamily; // family names
    int myFamily;                            // family index

    /* True if agent has been assigned to current timeslot */
    bool myAssigned;

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

    double cost(cTask *task) const
    {
        if( ! myTasks.size() )
            return 0;
        return myTasks[0].second;
    }

    int family() const
    {
        return myFamily;
    }

    bool cando( cTask* task ) const;

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

    bool isGroup()
    {
        return ( myName.find("_group") != -1 );
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

    static void sortAssignedCount();

    static void sortFamily(const cSlot *slot);

    /// get all the agents in sorted order

    static std::vector<cAgent *>
    get()
    {
        return theAgents;
    }

    /// get agents that can do task

    static std::vector<cAgent *>
    getForTask( cTask* task );

    static std::string specTextAll();


};

class cAgentGroup : public cAgent
{
    std::vector<cAgent *> myMember;

public:
    cAgentGroup(const std::vector<std::string> &vtoken);

    virtual void assign( int day );

    virtual bool isAssignedRecently( int day ) const;

    virtual std::vector<cAgent*> getMembers() const
    {
        return myMember;
    }

    virtual std::string text() const;

    virtual std::string specText() const;

    static void add(const std::vector<std::string> &vtoken);

};
