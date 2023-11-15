#include <inputbox.h>
#include "cGUI.h"

cGUI::cGUI()
    : fm(wex::maker::make()),
      tabs(wex::maker::make<wex::tabbed>(fm)),
      plProblem(wex::maker::make<wex::panel>(tabs)),
      plAssignments(wex::maker::make<wex::panel>(tabs)),
      lbSlotNameLabel(wex::maker::make<wex::label>(plAssignments)),
      lbSlotName(wex::maker::make<wex::label>(plAssignments)),
      bnSlotFirst(wex::maker::make<wex::button>(plAssignments)),
      bnSlotPrev(wex::maker::make<wex::button>(plAssignments)),
      bnSlotNext(wex::maker::make<wex::button>(plAssignments)),
      bnSlotLast(wex::maker::make<wex::button>(plAssignments)),
      rbMaxflow(wex::maker::make<wex::radiobutton>(plAssignments)),
      rbHungarian(wex::maker::make<wex::radiobutton>(plAssignments)),
      rbAgent2Tasks(wex::maker::make<wex::radiobutton>(plAssignments))
{

    // run example problem 1

    // allocator.example1();

    // // do the allocation
    // allocator.maxflow();
    // allocator.hungarian();
    // allocator.agents2tasks();

    fm.move({50, 50, 600, 700});
    fm.text("Agents2Tasks");

    // construct tabs

    tabs.move(0, 0, 600, 700);
    tabs.tabWidth(200);
    tabs.add("PROBLEM", plProblem);
    tabs.add("ASSIGNMENTS", plAssignments);

    // construct menus

    menuCTOR();

    // construct assignment tab

    plAssignCTOR();

    // handle problem tab draw event

    plProblem.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            plProblem.text(allocator.textProblem());
        });

    // initial display
    fm.show();
    tabs.select(0);

    // pass control to event handler
    fm.run();
}

void cGUI::menuCTOR()
{
    wex::menubar mbar(fm);

    // FILE menu

    wex::menu file(fm);
    file.append("Open",
                [&](const std::string &title)
                {
                    wex::filebox fb(fm);
                    auto fname = fb.open();
                    fm.text("Agents2Tasks " + fname);
                    readfile(allocator,fname);
                    allocator.maxflow();
                    allocator.hungarian();
                    Janusz(allocator);
                    tabs.select(0);
                });
    file.append("Save",
                [&](const std::string &title)
                {
                    wex::filebox fb(fm);
                    writefile(allocator,fb.save());
                });
    file.append("Example1",
                [&](const std::string &title)
                {
                    allocator.example1();

                    // do the allocation
                    allocator.maxflow();
                    allocator.hungarian();
                    Janusz( allocator );
                });
    file.append("Unit Test",
                [&](const std::string &title)
                {
                    if (!unitTest())
                    {
                        wex::msgbox mb("Unit test failed");
                        throw std::runtime_error("16 "
                            "Unit test failed");
                    }
                    else
                    {
                        wex::msgbox mb("Unit test passed");
                    }
                });
    mbar.append("File", file);

    // EDIT menu

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
                   ib.text("Task");
                   ib.add("Task type", "");
                   ib.showModal();
                   allocator.addTaskType(ib.value("Task type"));
                   allocator.maxflow();
                   allocator.hungarian();
                   fm.update();
               });
    add.append("Add agent",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.gridWidth(400);
                   ib.text("Agent");
                   ib.add("Name", "");
                   ib.add("Cost", "1.0");
                   ib.add("Tasks", "");
                   ib.showModal();
                   allocator.addAgent(
                       ib.value("Name"),
                       ib.value("Tasks"),
                       atof(ib.value("Cost").c_str()),
                       "none");
                   allocator.maxflow();
                   allocator.hungarian();
                   fm.update();
               });
    add.append("Add timeslot",
               [&](const std::string &title)
               {
                   wex::inputbox ib;
                   ib.text("Timeslot");
                   ib.gridWidth(400);
                   ib.add("Name", "");
                   ib.add("Tasks", "");
                   ib.showModal();
                   allocator.addSlot(
                       ib.value("Name"),
                       ib.value("Tasks"));
                   allocator.maxflow();
                   allocator.hungarian();
                   fm.update();
               });
    mbar.append("Edit", add);
}

void cGUI::plAssignCTOR()
{
    // widgets

    lbSlotNameLabel.move(100, 10, 90, 30);
    lbSlotNameLabel.text("Time Slot");
    lbSlotName.move(200, 10, 200, 30);
    int x = 50;
    bnSlotFirst.move(x, 50, 70, 30);
    x += 100;
    bnSlotPrev.move(x, 50, 70, 30);
    x += 100;
    bnSlotNext.move(x, 50, 70, 30);
    x += 100;
    bnSlotLast.move(x, 50, 70, 30);
    bnSlotFirst.text("FIRST");
    bnSlotPrev.text("PREV");
    bnSlotNext.text("NEXT");
    bnSlotLast.text("LAST");

    rbMaxflow.move(50, 100, 120, 30);
    rbMaxflow.text("Max Flow");
    rbHungarian.move(200, 100, 120, 30);
    rbHungarian.text("Hungarian");
    rbAgent2Tasks.move(350,100,120,30);
    rbAgent2Tasks.text("Agent2Task");
    rbAgent2Tasks.check();

    // event handlers

    bnSlotFirst.events().click(
        [this]()
        {
            allocator.setSlotFirst();
            plAssignments.update();
        });
    bnSlotPrev.events().click(
        [this]()
        {
            allocator.setSlotPrev();
            plAssignments.update();
        });
    bnSlotNext.events().click(
        [this]()
        {
            allocator.setSlotNext();
            plAssignments.update();
        });
    bnSlotLast.events().click(
        [this]()
        {
            allocator.setSlotLast();
            plAssignments.update();
        });
    rbMaxflow.events().click(
        [this]()
        {
            plAssignments.update();
        });
    rbHungarian.events().click(
        [this]()
        {
            plAssignments.update();
        });
    rbAgent2Tasks.events().click(
        [this]()
        {
            plAssignments.update();
        });

    plAssignments.events().draw(
        [this](PAINTSTRUCT &ps)
        {
            lbSlotName.text(allocator.slotName());
            std::string sass;
            switch (rbHungarian.checkedOffset())
            {
            case 0:
            default:
                sass = allocator.maxflowText();
                break;
            case 1:
                sass = allocator.hungarianText();
                break;
            case 2:
                sass = allocator.agents2TasksText();
                break;
            }
            wex::shapes S(ps);
            S.text(sass,
                   {5, 150, 600, 700});
        });
}