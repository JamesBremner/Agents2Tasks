/// @brief An agent can be assigned to tasks of certain work types

typedef std::chrono::system_clock::time_point timepoint_t;

class cAgent
{
    std::string myName;

    /// @brief tasks agent ready to do ( task type id, cost )
    std::vector<std::pair<int, double>> myTasks;

    /* true if assigned to task in current timeslot

    Prevent assigning an agent two tasks in one timeslot
    */

    bool fAssigned;

    /* number of task assignments in all timeslots so far

        This allows the total worload to be balanced between agents
        by assigning tasks to capable agents that have been assigned the least

        https://github.com/JamesBremner/Agents2Tasks/issues/5#issuecomment-1799230242
    */

    int myAssignedCount; 

    timepoint_t myLastAssignmentTime;   //  00:00:01 on day of previous assignment 

    static std::vector<std::string> vFamily;    // family group names
    int myFamily;                               // family group index

public:
    /// @brief Constructor
    /// @param name
    /// @param vt       // Indices of task types agent can do
    /// @param cost     // Cost of assigning agent to any task
    /// @param family   // Family group

    cAgent(
        const std::string &name,
        const std::vector<int> &vt,
        double cost,
        const std::string family );
    cAgent(
        const std::string &name,
        const std::vector<int> &vt,
        double cost )
        : cAgent( name, vt, cost, "none" )
        {}

    std::string name() const
    {
        return myName;
    }

    /// @brief true if agent can do task
    /// @param task task type id
    /// @return

    bool isAble(int task) const;

    double cost() const;

    void setPreviousTasks( int c )
    {
        myAssignedCount = c;
    }
    void unAssign()
    {
        fAssigned = false;
    }

    /// @brief Assign agent to task
    /// @param day integer representing day of slot
    /// @param taskTypeName task type name

    void assign(
        int day,
        const std::string &taskTypeName);

    bool isAssigned() const
    {
        return fAssigned;
    }

    /// @brief true if agent assigned recently to task of same type
    /// @param day
    /// @param taskname
    /// @return

    bool isAssignedRecently(
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

    void writefile(
        std::ofstream &ofs) const;
};
