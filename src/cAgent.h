/// @brief An agent can be assigned to tasks of certain work types

class cAgent
{
    std::string myName;

    /// @brief tasks agent ready to do ( task type id, cost )
    std::vector<std::pair<int, double>> myTasks;

    bool fAssigned; // true if assigned to task in current timeslot

    int myAssignedCount; // number of task assignments in all timeslots

    /// @brief previous assignments ( day, type name )
    std::vector<std::pair<int,std::string>> myAssignedDays;

public:
    /// @brief Constructor
    /// @param name
    /// @param vt       // Indices of task types agent can do
    /// @param cost     // Cost of assigning agent to any task

    cAgent(
        const std::string &name,
        const std::vector<int> &vt,
        double cost);

    std::string name() const
    {
        return myName;
    }

    /// @brief true if agent can do task
    /// @param task task type id
    /// @return

    bool isAble(int task) const;

    double cost() const;

    void unAssign()
    {
        fAssigned = false;
    }

    /// @brief Assign agent to task
    /// @param day integer representing day of slot
    /// @param taskTypeName task type name

    void assign(
        int day,
        const std::string& taskTypeName);

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
         const std::string& taskname ) const;

    int assignedCount() const
    {
        return myAssignedCount;
    }

    std::string text(
        const std::vector<std::string> vTaskType) const;

    void writefile(
        std::ofstream &ofs,
        const cAllocator &allocator);
};
