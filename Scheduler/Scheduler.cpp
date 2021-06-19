#include <iostream>
#include <cassert>
#include "Scheduler.h"
#include <windows.h>

using std::cout;
using std::endl;

Scheduler::Scheduler()
	: m_controller(NativeThreadController())
{
}

void Scheduler::add_thread(Thread& thread)
{
	if (thread.state == Thread::State::Ready) {
		add_to_ready(thread);
	}
	else {
		m_blocked.insert(&thread);
	}
}

// Inserts the thread into the right position in the ready queue
void Scheduler::add_to_ready(Thread& thread)
{
	if (m_ready.empty()) {
		m_ready.push_back(&thread);
		return;
	}

	// Insert after the last task of the same priority and before the first task of higher priority
	// Presume the queue is already sorted and correct
	for (auto it = m_ready.begin(); it != m_ready.end(); it++) {
		if (thread > (**it)) {
			m_ready.insert(it, &thread);	// Insert before the first item with a lower priority
			return;
		}
	}

	m_ready.push_back(&thread);
}

void Scheduler::pause_currently_executing()
{
	m_controller.pause(m_currently_executing->native_thread);

	m_currently_executing->executing_for = 0;
	m_ready.pop_front();
	add_to_ready(*m_currently_executing);
}

void Scheduler::update_and_start_currently_executing()
{
	Thread* thread = *m_ready.begin();
	cout << "Scheduler::update_and_start_currently_executing(\"" << thread->name << "\")" << endl;

	m_currently_executing = thread;
	m_controller.resume(m_currently_executing->native_thread);
}

void Scheduler::run()
{
	SetThreadPriority(GetCurrentThread(), SCHEDULER_PRIORITY);	// TODO: Use NativeThread to have run() in a separate thread

	while (true) {
		Sleep(m_tick_interval_ms);

		cout << "Scheduler::run()" << endl;

		// Move from blocked to ready if unblocked
		// Select the highest priority task and run it
		if (m_ready.empty()) {
			continue;
		}

		// Nothing is executing -- start executing the highest priority thread
		if (!m_currently_executing) {
			update_and_start_currently_executing();
			continue;
		}

		// Currently executing thread has ran over it's time slice allowment, move it to the back of the queue and start executing a new thread
		m_currently_executing->executing_for += 1;
		if (m_currently_executing->executing_for >= m_time_slice_period) {
			pause_currently_executing();
			update_and_start_currently_executing();
			continue;
		}
	}
}