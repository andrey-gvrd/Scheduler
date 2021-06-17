#pragma once

/*
 Features

	 Pre-emptive
		 Higher priority tasks will always run before the lower priority ones

	 Time Slicing
		 Tasks of equal priority take turns executing for a fixed time slice interval (multiple of ticks)

 Implementation Details

	 Ready Tasks
		One sorted queue. After execution, a task is inserted back after the last task of the same priority, but before the task of the lower priority

	 Ready -> Blocked
		 sleep()
			The thread signals to the scheduler that it needs to be put into the non-running list until current_time + sleep_time
		 wait_for_response()
			-''- until a data is put into a shared location (?)

	 Blocked -> Ready
		 Time-based (sleep())
			Scheduler checks the wake time, if lower than current, move to the ready queue
		 Signal-based (wait_for_response())
			A. The data-sharing-object signals to the scheduler
			B. -''- modifies the state of the task, so the scheduler can move it to the ready queue later

	 Pre-emption
		Scheduler runs in the high-priority main thread
*/

/*
TODO
	[x] Add windows threads as the base
	[ ] Separate windows threads using an interface
*/

#include <list>
#include <set>
#include "NativeThread.h"
#include "Thread.h"

class Scheduler {
	std::list<Thread*> m_ready{ };
	std::set<Thread*> m_blocked{ };
	Thread* m_currently_executing{ nullptr };
	NativeThreadController m_controller;

	unsigned m_time_slice_period = 4;
	unsigned m_tick_interval_ms = 1000;
public:
	Scheduler();
	void add_thread(Thread&);
	void run();
private:
	void add_to_ready(Thread&);
	void pause_currently_executing();
	void update_and_start_currently_executing();
};
