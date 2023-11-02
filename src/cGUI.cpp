#include <inputbox.h>
#include "cGUI.h"

cGUI::cGUI()
    : fm(wex::maker::make()),
      tabs(wex::maker::make<wex::tabbed>(fm)),
      plProblem(wex::maker::make<wex::panel>(tabs)),
      plMaxFlow(wex::maker::make<wex::panel>(tabs)),
      plHungarian(wex::maker::make<wex::panel>(tabs))
{

    allocator.addTaskType("teacher");
    allocator.addTaskType("cleaner");
    allocator.addTaskType("accountant");
    allocator.addAgent(
        "John",
        {"teacher cleaner"},
        3.0);
    allocator.addAgent(
        "Margaret",
        {"accountant cleaner"},
        4.0);
    allocator.addAgent(
        "Andrew",
        {"accountant teacher"},
        5.0);
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
    allocator.allocateMaxFlow();
    allocator.hungarian();

    fm.move({50, 50, 600, 700});
    fm.text("Agents2Tasks");

    tabs.move(0, 0, 600, 700);
    tabs.tabWidth(200);
    tabs.add("PROBLEM", plProblem);
    tabs.add("MAX FLOW", plMaxFlow);
    tabs.add("HUNGARIAN", plHungarian);

    menuCTOR();

    plProblem.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            plProblem.text(allocator.textProblem());
        });
    plMaxFlow.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            plMaxFlow.text(allocator.textMaxflow());
        });
    plHungarian.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            plHungarian.text(allocator.hungarianText());
        });

    fm.show();
    tabs.select(0);
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
                   allocator.addTaskType(ib.value("Task type"));
                   fm.update();
               });
    add.append("Add agent",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.gridWidth(400);
                   ib.add("Name", "");
                   ib.add("Cost", "1.0");
                   ib.add("Tasks", "");
                   ib.showModal();
                   allocator.addAgent(
                       ib.value("Name"),
                       ib.value("Tasks"),
                       atof(ib.value("Cost").c_str()));
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