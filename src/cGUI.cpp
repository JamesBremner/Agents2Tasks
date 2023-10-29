    #include "cGUI.h"
    
    cGUI::cGUI(
        const std::string &title,
        const std::vector<int> &vlocation)
        : fm(wex::maker::make()),
        myPanel(wex::maker::make<wex::panel>(fm))
    {
        allocator.addTask("teacher");
        allocator.addTask("cleaner");
        allocator.addTask("accountant");
        allocator.addSlot(
            "28/OCT/2023 8:30",
            {"teacher", "teacher", "cleaner"});
        allocator.addSlot(
            "29/OCT/2023 10:00",
            { "teacher", "accountant"});
        allocator.addSlot(
            "2/NOV/2023 8:30",
            { "teacher", "teacher", "accountant"});
        allocator.addSlot(
            "3/NOV/2023 10:00",
             { "teacher", "accountant"});
        

        fm.move(vlocation);
        fm.text(title);
        myPanel.move({0,0,500,500});

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });

        fm.show();
        fm.run();
    }