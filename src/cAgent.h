
class cAgent
{
protected:
    static std::vector<cAgent *> theAgents;

    std::string myName;

    std::vector<std::pair<cTask *, double>> myTasks;

    static std::vector<std::string> vFamily; // family group names
    int myFamily;                            // family group index

    bool myAssigned;

    timepoint_t myLastAssignmentTime; //  00:00:01 on day of previous assignment

    int myAssignedCount;

public:
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
        return myTasks[0].second;
    }

    int family() const
    {
        return myFamily;
    }

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

    std::string text() const;

    static void clear()
    {
        for (auto *pa : theAgents)
            delete pa;
        theAgents.clear();
    }

    /// @brief Add an agent
    /// @param vtoken tokenized specification line

    static void add(const std::vector<std::string> &vtoken);

    static void unassignAll();

    static cAgent* find( const std::string& name );

    static void sortAssignedCount();

    static void sortFamily(const cSlot *slot);

    static std::vector<cAgent *>
    getAll()
    {
        return theAgents;
    }

    static std::string specText();
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

    static void add(const std::vector<std::string> &vtoken);

};
