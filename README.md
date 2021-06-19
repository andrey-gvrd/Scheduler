## Scheduler (WIP)

This scheduler is a written in C++ as a design exercise.

### Features

- 3 priority levels
- Pre-emption
- Time-slicing

### Implementation

<div style="overflow: hidden; height: 100%;">
    <img float="left" height="450" width="auto" src="images/scheduler-diagram.png" />
</div>


### UserThread / Threads

`UserThread` is a user-defined class that inherits from `Thread` and is the main interface point for your application. The only thing you need to get started is implement a non-returning `Thread::handle()` and add `using Thread::Thread;` to use `Thread`'s constructor.

### Scheduler

Once all the `UserThread`s are added to the scheduler with `Scheduler::add_thread()`, calling `Scheduler::run()` takes control of the main thread. The scheduler is implemented with a priority queue for ready threads and a pool of blocked threads. At every tick, the scheduler:

1. Updates the states of the threads

2. Moves the the threads between the blocked and ready sets

3. Switches execution to the thread at the front of the queue

When multiple threads of the same priority are running at the same time, the scheduler cycles through them by putting them after the last thread of the same priority after they've ran through their time-slice.

### Native Threads

The scheduler currently uses Windows threading API to implement context switching between threads at arbitrary points of time. This works by using 4 native priorities:

- Highest - for the scheduler

- High - for the currently executing thread

- Medium - for the idle thread

- Low - for all the other threads

There's two types of system threads: the scheduler and the idle thread

The execution starts with all the user threads assigned the Low priorities. In this state, only the system threads are running. The idle thread is running constantly, except when preempted by the scheduler at regular intervals. The idle thread is used as a stop-gap that's preventing the Windows from running the currently inactive threads.

When the scheduler first decides to start executing a thread, it raises its native priority above the idle thread, which causes the native threading API to run it.

When the scheduler decides to start executing a different thread, it lowers the native priority of the currently running thread and raises the native priority of the new thread.