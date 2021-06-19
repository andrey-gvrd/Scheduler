#pragma once

#include <list>
#include <set>
#include "NativeThread.h"
#include "Thread.h"

class Scheduler {
	std::list<Thread*> m_ready{ };
	std::set<Thread*> m_blocked{ };
	Thread* m_currently_executing{ nullptr };
	NativeThreadController m_controller;

	unsigned m_time_slice_period = 4;	// TODO: Make passable
	unsigned m_tick_interval_ms = 1000;	// TODO: Make passable
public:
	Scheduler();
	void add_thread(Thread&);
	void run();
private:
	void add_to_ready(Thread&);
	void pause_currently_executing();
	void update_and_start_currently_executing();
};
