#include <string>
#include <vector>

class cAgent
{
protected:

    static std::vector<cAgent *> theAgents;

    std::string myName;

    std::vector<std::pair<cTask*, double>> myTasks;

    static std::vector<std::string> vFamily; // family group names
    int myFamily;                            // family group index

    bool myAssigned;

    int myAssignedCount;

public:

    /// CTOR
    /// @param vtoken tokenized specification line

    cAgent(const std::vector<std::string> &vtoken);

    void assign( bool f = true );

    void setPreviousTasks( int c )
    {
        myAssignedCount = c;
    }

    std::string name() const
    {
        return myName;
    }

    double cost( cTask* task ) const
    {
        return myTasks[0].second;
    }

    int family() const{
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

    static void sortAssignedCount();

    static void sortFamily(const cSlot* slot);

    static std::vector<cAgent *>
    getAll()
    {
        return theAgents;
    }

    static std::string specText();
};
