# Agents2Tasks

Allocate agents to tasks in multiple timeslots.


## Problem specifcations

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/25c3d5a4-8316-462f-8bbf-0c66e4cbd677)

## MaxFlow Assignment

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/fa074f44-8cbe-4885-bc46-13a779c2152c)

## Hungarian Assignment

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/0712c46f-fae4-4a5b-801d-9591721f6185)

## File Menu

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/d3c8e81c-4020-4cb4-83e0-272b24a80b1b)

`File | Open`  Load problem specifcation from a file

`File | Save`  Save problem specifications to a file

`File | Example1` Specify problem example 1 specifications

The [file format](https://github.com/JamesBremner/Agents2Tasks/wiki/File-format) documentation

## Edit Menu

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/a757bd30-3c70-440c-8ff4-deff422cbac3)

`Edit | Clear` Clear problem specifications, ready to enter new specs from GUI.

`Edit | Add task type`  Use GUI to add a task type.

`Edit |  Add Agent` Add an agent with assignment cost and task types the agent can be assigned to, using the GUI.

`Edit | Timeslot` Add a timeslot, along with the task types required, using the GUI.


# Algorithm

Maximum flow ( [Edmonds–Karp](https://en.wikipedia.org/wiki/Edmonds%E2%80%93Karp_algorithm) ) maximises the number of tasks that are allocated in each timeslot.

The [Hungarian algorithm]( https://en.wikipedia.org/wiki/Hungarian_algorithm) is used to minimize the cost in each timeslot.





