#include <inputbox.h>
#include "cGUI.h"

cGUI::cGUI()
    : fm(wex::maker::make()),
      tabs(wex::maker::make<wex::tabbed>(fm)),
      plProblem(wex::maker::make<wex::panel>(tabs)),
      plMaxFlow(wex::maker::make<wex::panel>(tabs)),
      plHungarian(wex::maker::make<wex::panel>(tabs))
{

    allocator.example1();

    // do the allocation
    allocator.maxflow();
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

    wex::menu file(fm);
    file.append("Open",
                [&](const std::string &title)
                {
                    wex::filebox fb(fm);
                    auto fname = fb.open();
                    fm.text("Agents2Tasks " + fname);
                    allocator.readfile(fname);
                    allocator.maxflow();
                    allocator.hungarian();
                    tabs.select(0);
                });
    file.append("Save",
                [&](const std::string &title)
                {
                    wex::filebox fb(fm);
                    auto fname = fb.save();
                    allocator.writefile(fname);
                });
    file.append("Example1",
                [&](const std::string &title)
                {
                    allocator.example1();

                    // do the allocation
                    allocator.maxflow();
                    allocator.hungarian();
                });
    mbar.append("File", file);

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
                       atof(ib.value("Cost").c_str()));
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