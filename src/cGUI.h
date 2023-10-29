#include <wex.h>
#include "allocator.h"

class cGUI
{
public:
    /** CTOR
     * @param[in] title will appear in application window title
     * @param[in] vlocation set location and size of appplication window
     */

    cGUI(
        const std::string &title,
        const std::vector<int> &vlocation);

    virtual void draw(wex::shapes &S)
    {
        myPanel.text( allocator.text() );
    }

private:
    wex::gui &fm;
    wex::panel & myPanel;

    cAllocator allocator;
};