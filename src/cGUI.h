#include <wex.h>
#include "allocator.h"

class cGUI
{
public:

    cGUI();

private:
    wex::gui &fm;
    wex::tabbed& tabs;
    wex::panel& plProblem;
    wex::panel& plHungarian;
    wex::label& lbSlotNameLabel;
    wex::label& lbSlotName;
    wex::button& bnSlotFirst;
    wex::button& bnSlotPrev;
    wex::button& bnSlotNext;
    wex::button& bnSlotLast;
    wex::radiobutton& rbMaxflow;
    wex::radiobutton& rbHungarian;

    cAllocator allocator;

    void menuCTOR();
    void plHungarianCTOR();
};