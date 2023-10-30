#include <inputbox.h>
#include "cGUI.h"

cGUI::cGUI()
    : fm(wex::maker::make()),
      myPanel(wex::maker::make<wex::panel>(fm))
{

    allocator.addTask("teacher");
    allocator.addTask("cleaner");
    allocator.addTask("accountant");
    allocator.addAgent(
        "John",
        {"teacher cleaner"});
    allocator.addAgent(
        "Margaret",
        {"accountant cleaner"});
    allocator.addAgent(
        "Andrew",
        {"accountant teacher"});
    allocator.addSlot(
        "28/OCT/2023 8:30",
        {"teacher teacher cleaner"});
    allocator.addSlot(
        "29/OCT/2023 10:00",
        {"teacher accountant"});
    allocator.addSlot(
        "2/NOV/2023 8:30",
        {"teacher teacher accountant"});
    allocator.addSlot(
        "3/NOV/2023 10:00",
        {"teacher accountant"});

    // do the allocation
    allocator.allocate();

    fm.move({50,50,500,600});
    fm.text("Agents2Tasks");
    myPanel.move({0, 0, 500, 700});

    menuCTOR();

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    fm.show();
    fm.run();
}

void cGUI::menuCTOR()
{
    wex::menubar mbar(fm);
    wex::menu add(fm);
    add.append("Clear",
               [&](const std::string &title)
               {
                   allocator.clear();
                   fm.update();
               });
    add.append("Add task type",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.gridWidth(400);
                   ib.add("Task type", "");
                   ib.showModal();
                   allocator.addTask(ib.value("Task type"));
                   fm.update();
               });
    add.append("Add agent",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.gridWidth(400);
                   ib.add("Name", "");
                   ib.add("Tasks", "");
                   ib.showModal();
                   allocator.addAgent(
                       ib.value("Name"),
                       ib.value("Tasks"));
                   fm.update();
               });
    add.append("Add timeslot",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.gridWidth(400);
                   ib.add("Name", "");
                   ib.add("Tasks", "");
                   ib.showModal();
                   allocator.addSlot(
                       ib.value("Name"),
                       ib.value("Tasks"));
                   fm.update();
               });
    mbar.append("Edit", add);
}