/// @brief A task, unique to its timeslot, of a certain type
class cTask
{
    int myTaskType;
    int myIndex;
    bool fAssigned;
    static std::vector<std::string> vTaskType; // task type names

public:

    cTask(int iType, int i)
        : myTaskType(iType),
          myIndex(i),
          fAssigned(false)
    {
    }
    static void clear()
    {
        vTaskType.clear();
    }

    void assign()
    {
        fAssigned = true;
    }
    static void addTaskType(
        const std::string &stype);

    /// @brief get task type idex from name
    /// @param name 
    /// @return 
    ///
    /// if the name is absent, add it

    static int findType(const std::string &name);

    int taskType() const
    {
        return myTaskType;
    }

    std::string typeName() const
    {
        return vTaskType[ myTaskType ];
    }

    static std::string typeName( int i )
    {
        return vTaskType[i];
    }

    static const std::vector<std::string>&
    getTypeNames()
    {
        return vTaskType;
    }

    static std::string typeText();
};
