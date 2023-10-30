# Agents2Tasks

Allocate agents to tasks in multiple timeslots.

Construct instance of class to allocate agents to tasks

```C++
    cAllocator allocator;
```

Construct tasks of various types

```C++
allocator.addTask("teacher");
allocator.addTask("cleaner");
allocator.addTask("accountant");
```

Construct agents, each able to do multiple task types

```C++
allocator.addAgent(
    "John",
    {"teacher", "cleaner"});
allocator.addAgent(
    "Margaret",
    {"accountant", "cleaner"});
allocator.addAgent(
    "Andrew",
    {"accountant", "teacher"});
```

Construct timeslots with tasks that must be done

```C++
allocator.addSlot(
    "28/OCT/2023 8:30",
    {"teacher", "teacher", "cleaner"});
allocator.addSlot(
    "29/OCT/2023 10:00",
    {"teacher", "accountant"});
allocator.addSlot(
    "2/NOV/2023 8:30",
    {"teacher", "teacher", "accountant"});
allocator.addSlot(
    "3/NOV/2023 10:00",
    {"teacher", "accountant"});
```

Run the allocation and display the results

```C++
 allocator.allocate();

std::cout << allocator.text();
```

The output is

![image](https://github.com/JamesBremner/Agents2Tasks/assets/2046227/7d58cf20-7d03-47eb-aea4-883559b6e778)

# Algorithm

Maximum flow maximises the number of tasks that are allocated in each timeslot.

The Hungarian algorithm is used to minimize the cost in each timeslot.





