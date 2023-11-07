# Agents2Tasks

Allocate agents to tasks in multiple independant timeslots.


## Problem specifcations

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/49ba7a04-b0e0-4beb-a685-1eeeceac079c)



## Assignment

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/24e90f9c-a438-43c5-8ad3-895d028bd2a5)

Display shows assignments of agents to tasks one timeslot at a time.  

Buttons allow user to navigate between timeslots.  

Radiobuttons allow user to switch between results from the optional algorithms: Maxflow, Hungarian or A2T.

## File Menu

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/80208e92-4b69-4b3b-8f7d-99bda8cde22e)

`File | Open`  Load problem specifcation from a file

`File | Save`  Save problem specifications to a file

`File | Example1` Run example problem 1

`File | Unit Test` Run unit test.

The [file format](https://github.com/JamesBremner/Agents2Tasks/wiki/File-format) documentation

## Edit Menu

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/a757bd30-3c70-440c-8ff4-deff422cbac3)

`Edit | Clear` Clear problem specifications, ready to enter new specs from GUI.

`Edit | Add task type`  Use GUI to add a task type.

`Edit |  Add Agent` Add an agent with assignment cost and task types the agent can be assigned to, using the GUI.

`Edit | Timeslot` Add a timeslot, along with the task types required, using the GUI.


# Algorithm

Maximum flow ( [Edmonds–Karp](https://en.wikipedia.org/wiki/Edmonds%E2%80%93Karp_algorithm) ) maximises the number of tasks that are allocated in each timeslot.

The [Hungarian algorithm]( https://en.wikipedia.org/wiki/Hungarian_algorithm) is used to minimize the cost in each timeslot using the classic Hungarian cost matrix.

The Agent2Task algorithm uses a strightforward assignment of the cheapest agent to each task in turn.





