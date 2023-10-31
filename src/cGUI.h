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
    wex::panel& plMaxFlow;
    wex::panel& plHungarian;

    cAllocator allocator;

    void menuCTOR();
};