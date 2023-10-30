#include <wex.h>
#include "allocator.h"

class cGUI
{
public:

    cGUI();

    virtual void draw(wex::shapes &S)
    {
        myPanel.text( allocator.text() );
    }

private:
    wex::gui &fm;
    wex::panel & myPanel;

    cAllocator allocator;

    void menuCTOR();
};