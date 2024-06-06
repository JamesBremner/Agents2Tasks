
class cSlot
{
    std::string myName;

    std::vector< cTask > myTasks;

    std::set<int> myFamily; // indices of family groups assigned to this slot

public:
    cSlot(const std::vector<std::string> &vtoken);

    std::string name() const
    {
        return myName;
    }

    std::string text() const;

    std::vector<cTask> &getTasks()
    {
        return myTasks;
    }

    bool hasFamily(int family) const;

    /// @brief get integer representation for day containing timeslot
    /// @return integer day
    ///
    /// Assumes slot name format YYYYMMDDHHMM
    /// returns int( YYYYMMDD )

    int day() const
    {
        return atoi(myName.substr(0, 8).c_str());
    }

    cTask& firstUnassigned( cTaskType* pt);

    void assign(int iFamily)
    {
        myFamily.insert(iFamily);
    }

    bool isTaskAssigned( int kTask ) const
    {
        return myTasks[kTask].isAssigned();
    }
    void taskAssign( int kTask )
    {
        myTasks[kTask].assign();
    }

    static void clear();

    static void add(
        std::vector<std::string> &vtoken);

    static bool isSane();

    static std::vector<cSlot *>
    get();

    static std::string specText();
};
