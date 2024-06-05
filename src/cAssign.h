#pragma once

class cAssign
{

    cSlot *mySlot;
    cAgent *myAgent;
    cTaskType *myTask;
    cAgentGroup * myGroup;

public:

    // construct assignment of single agent
    cAssign(cSlot *ps, cAgent *pa, cTaskType *pt);

    // construct assignment of group agent member
    cAssign(cSlot *ps, cAgent *pa, cTaskType *pt, cAgentGroup* pg );

    void set( cAgentGroup* g )
    {
        myGroup = g;
    }

    cSlot* slot() const{
        return mySlot;
    }

    /// @brief Get display text for this assignment
    /// @return 

    std::string text(const std::string& slotName) const;

    static void clear();

    /// @brief Assign agent to task in slot
    /// @param ps slot
    /// @param pa agent
    /// @param pt task type

    static void add(cSlot *ps, cAgent *pa, cTaskType *pt );
    static void addIndividual(cSlot *ps, cAgent *pa, cTaskType *pt );


    /// @brief Get display text for the assignments in a slot
    /// @param pSlot 
    /// @return 

    static std::string text(cSlot *pSlot);

    /// @brief get assignments in a timeslot
    /// @param slot 
    /// @return 

    static std::vector<cAssign *>
    getSlotAssigns( cSlot* slot );

};

void Agents2Tasks( bool fexplain );
