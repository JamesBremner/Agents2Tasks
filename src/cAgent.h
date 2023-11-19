/// @brief An agent can be assigned to tasks of certain work types

typedef std::chrono::system_clock::time_point timepoint_t;

class cAgent
{
    public:

    enum class eAssign {
        none,
        agent,
        group,
    };

    protected:

    cAllocator& allocator;

    std::string myName;

    /// @brief tasks agent ready to do ( task type id, cost )
    std::vector<std::pair<int, double>> myTasks;

    /* true if assigned to task in current timeslot

    Prevent assigning an agent two tasks in one timeslot
    */

    eAssign myAssign;

    /* number of task assignments in all timeslots so far

        This allows the total worload to be balanced between agents
        by assigning tasks to capable agents that have been assigned the least

        https://github.com/JamesBremner/Agents2Tasks/issues/5#issuecomment-1799230242
    */

    int myAssignedCount;

    timepoint_t myLastAssignmentTime; //  00:00:01 on day of previous assignment

    static std::vector<std::string> vFamily; // family group names
    int myFamily;                            // family group index

public:

    /// @brief Constructor
    /// @param name
    /// @param vt       // Indices of task types agent can do
    /// @param cost     // Cost of assigning agent to any task
    /// @param family   // Family group

    cAgent(
        cAllocator& A,
        const std::string &name,
        const std::vector<int> &vt,
        double cost,
        const std::string family);
    cAgent(
        cAllocator& A,
        const std::string &name,
        const std::vector<int> &vt,
        double cost)
        : cAgent(A,name, vt, cost, "none")
    {
    }

    std::string name() const
    {
        return myName;
    }

    /// @brief true if agent can do task
    /// @param task task type id
    /// @return

    bool isAble(int task) const;

    double cost() const;

    void setPreviousTasks(int c)
    {
        myAssignedCount = c;
    }
    void unAssign()
    {
        myAssign = eAssign::none;
    }

    /// @brief Assign agent to task
    /// @param day integer representing day of slot
    /// @param taskTypeName task type name

    virtual void assignTask(
        int day,
        const std::string &taskTypeName,
        slotsolution_t& slotsolution );

    bool isAssigned() const
    {
        return myAssign != eAssign::none;
    }

    /// @brief true if agent assigned recently to task of same type
    /// @param day
    /// @param taskname
    /// @return

    virtual bool isAssignedRecently(
        int day,
        const std::string &taskname) const;

    int assignedCount() const
    {
        return myAssignedCount;
    }

    int family() const
    {
        return myFamily;
    }

    std::string text() const;

    virtual void writefile(
        std::ofstream &ofs,
             const cAllocator& A) const;

    std::string logText() const;
};

class cAgentGroup : public cAgent
{
    /* Names of agents who are members of the group
    
    Names are stored, rather than indices
    because agent sorting moves the agents around to different indices

    TODO change agent IDs to be independant of location in the allocator::myAgent vector
    */

    std::vector<std::string> myAgent;   // IDs of agents in group

    public:

    cAgentGroup(
        cAllocator& A,
        const std::vector<std::string>& vtoken,
        int taskID );

    void add( const std::string& agentName)
    {
        myAgent.push_back( agentName );
    }

     virtual bool isAssignedRecently(
        int day,
        const std::string &taskname) const;

    virtual void assignTask(
        int day,
        const std::string &taskTypeName,
        slotsolution_t& slotsolution);

    virtual void writefile(
        std::ofstream &ofs,
             const cAllocator& A) const;

    std::vector<std::string>& getMembers()
    {
        return myAgent;
    }
    
};
