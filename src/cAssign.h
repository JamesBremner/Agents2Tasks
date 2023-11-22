#include <string>
#include <vector>

class cAssign
{

    static std::vector<cAssign *> theAssigns;

    cSlot *mySlot;
    cAgent *myAgent;
    cTask *myTask;

public:
    cAssign(cSlot *ps, cAgent *pa, cTask *pt)
        : myAgent(pa),
          myTask(pt),
          mySlot(ps)
    {
        pa->assign();
    }

    cSlot* slot() const{
        return mySlot;
    }

    std::string text() const
    {
        std::stringstream ss;

        ss << myAgent->name() << " to " << myTask->name();
        return ss.str();
    }

    static void clear()
    {
        for (auto *pa : theAssigns)
            delete pa;
        theAssigns.clear();
    }

    static void add(cSlot *ps, cAgent *pa, cTask *pt)
    {
        theAssigns.push_back(
            new cAssign(ps, pa, pt));
    }

    static std::string text(cSlot *pSlot);

    /// @brief get assignments in a timeslot
    /// @param slot 
    /// @return 
    
    static std::vector<cAssign *>
    getSlotAssigns( cSlot* slot );

};

void assign();
