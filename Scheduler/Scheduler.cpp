#include <iostream>
#include <cassert>
#include "Scheduler.h"

using std::cout;
using std::endl;

DWORD WINAPI idle_thread_impl(LPVOID lpParam) {
	while (true) {
		//cout << "idle_thread_impl()" << endl;
	}
}

Scheduler::Scheduler()
{
	idle_thread = CreateThread(nullptr, 0, idle_thread_impl, nullptr, CREATE_SUSPENDED, nullptr);
	if (!idle_thread) {
		cout << "CreateThread(\"idle\") has failed." << endl;
		while (true);
	}

	if (!SetThreadPriority(idle_thread, THREAD_PRIORITY_NORMAL)) {
		cout << "SetThreadPriority(\"idle\") has failed." << endl;
		while (true);
	}
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

void Scheduler::stop_currently_executing()
{
	SetThreadPriority(m_currently_executing->win_handle, THREAD_PRIORITY_NORMAL);
}

void Scheduler::set_and_start_currently_executing(Thread& thread)
{
	cout << "Scheduler::set_and_start_currently_executing(\"" << thread.name << "\")" << endl;

	m_currently_executing = &thread;
	SetThreadPriority(m_currently_executing->win_handle, THREAD_PRIORITY_ABOVE_NORMAL);
}

void Scheduler::run()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	cout << "Resuming the idle and all the ready and blocked threads." << endl;

	ResumeThread(idle_thread);
	for (auto& thread : m_ready) {
		ResumeThread(thread->win_handle);
	}
	for (auto& thread : m_blocked) {
		ResumeThread(thread->win_handle);
	}

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
			set_and_start_currently_executing(**m_ready.begin());
			continue;
		}

		// Currently executing thread has ran over it's time slice allowment, move it to the back of the queue and start executing a new thread
		m_currently_executing->executing_for += 1;
		if (m_currently_executing->executing_for >= m_time_slice_period) {
			stop_currently_executing();

			Thread* thread = *m_ready.begin();

			assert(thread == m_currently_executing);

			thread->executing_for = 0;
			m_ready.pop_front();
			add_to_ready(*thread);

			set_and_start_currently_executing(**m_ready.begin());
			continue;
		}
	}
}