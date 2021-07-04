#include <iostream>
#include <cassert>
#include "Scheduler.h"
#include <windows.h>

using std::cout;
using std::endl;

void idle_thread()
{
	while (true) {
		//cout << "idle" << endl;
	}
}

Scheduler::Scheduler()
	: m_idle(NativeThread(static_cast<std::function<void()>>(idle_thread), IDLE_THREAD_PRIORITY, "idle_thread"))
{
	// Force single core execution to use thread priorities to control thread execution
	DWORD dwProcessAffinityMask = 1u;
	BOOL res = SetProcessAffinityMask(
		GetCurrentProcess(),
		dwProcessAffinityMask
	);

	if (!res) {
		cout << "SetProcessAffinityMask() failed" << endl;
		while (true);
	}

	DWORD lpProcessAffinityMask;
	DWORD lpSystemAffinityMask;
	res = GetProcessAffinityMask(
		GetCurrentProcess(),
		&lpProcessAffinityMask,
		&lpSystemAffinityMask
	);

	if (res) {
		//cout << "ProcessAffinityMask: " << lpProcessAffinityMask << endl;
		//cout << "lpSystemAffinityMask: " << lpSystemAffinityMask << endl;
	}
	else {
		cout << "GetProcessAffinityMask() failed" << endl;
		while (true);
	}

	SetThreadPriority(GetCurrentThread(), SCHEDULER_PRIORITY);	// TODO: Use NativeThread to have run() in a separate thread
}

void Scheduler::add_thread(Thread& thread)
{
	if (thread.m_state == Thread::State::Ready) {
		add_to_ready(thread);
	}
	else {
		m_blocked.insert(&thread);
	}
}

// Inserts the thread into the right position in the ready queue
void Scheduler::add_to_ready(Thread& thread)
{
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
	m_currently_executing->m_native_thread.pause();

	m_currently_executing->m_executing_for = 0;
	m_ready.pop_front();
	add_to_ready(*m_currently_executing);
}

void Scheduler::update_and_start_currently_executing()
{
	Thread* thread = *m_ready.begin();
	cout << "Scheduler::update_and_start_currently_executing(\"" << thread->m_name << "\")" << endl;

	m_currently_executing = thread;
	m_currently_executing->m_native_thread.resume();
}

void Scheduler::run()
{
	while (true) {
		Sleep(m_tick_interval_ms);

		cout << "Scheduler::run()" << endl;

		// Blocked -> Ready for newly woken threads
		for (auto it = m_blocked.begin(); it != m_blocked.end();) {
			Thread& thread = **it;
			if (thread.m_sleep_for == 0) {
				cout << thread.m_name << ": m_blocked -> m_ready" << endl;

				add_to_ready(thread);
				it = m_blocked.erase(it);
			}
			else {
				it++;
			}
		}

		// Ready -> Blocked for newly sleeping threads
		for (auto it = m_ready.begin(); it != m_ready.end(); it++) {
			Thread& thread = **it;
			if (thread.m_sleep_for > 0) {
				cout << thread.m_name << ": m_ready -> m_blocked" << endl;

				if (&thread == m_currently_executing) {
					m_currently_executing = nullptr;
				}
				thread.m_native_thread.pause();
				thread.m_executing_for = 0;

				m_blocked.insert(&thread);
				it = m_ready.erase(it);
			}
		}

		// Sleep the sleeping threads
		for (auto it = m_blocked.begin(); it != m_blocked.end(); it++) {
			Thread& thread = **it;
			if (thread.m_sleep_for > 0) {
				thread.m_sleep_for--;

				cout << thread.m_name << ": m_sleep_for = " << thread.m_sleep_for << endl;
			}
		}

		if (m_ready.empty()) {
			continue;
		}

		// Nothing is executing -- start executing the highest priority thread
		if (!m_currently_executing) {
			update_and_start_currently_executing();
			continue;
		}

		// Currently executing thread has ran over it's time slice allowment, move it to the back of the queue and start executing a new thread
		m_currently_executing->m_executing_for += 1;
		if (m_currently_executing->m_executing_for >= m_time_slice_period) {
			pause_currently_executing();
			update_and_start_currently_executing();
			continue;
		}
	}
}