#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <set>
#include <optional>
#include <thread>
#include <list>
#include <cassert>
#include <windows.h>

using std::cout;
using std::endl;

struct Thread {
	enum class State {
		Ready,
		Blocked
	};
	enum class Priority {
		Low = 0,
		Medium = 1,
		High = 2
	};

	Priority priority;
	State state;
	std::string name;
	std::function<void()> function;
	HANDLE win_handle;

	unsigned executing_for { 0 };

	Thread(std::string, std::function<void()>, Priority);

	bool operator<(const Thread& rhs) { return static_cast<int>(priority) > static_cast<int>(rhs.priority); }
	bool operator>(const Thread& rhs) { return rhs.priority < priority; }
};

DWORD WINAPI function_impl(LPVOID lpParam) {
	static_cast<Thread*>(lpParam)->function();
	return 0;
}

Thread::Thread(std::string _name, std::function<void()> _function, Thread::Priority _priority)
	: name(_name)
	, function(_function)
	, priority(_priority)
	, state(Thread::State::Ready)
{
	win_handle = CreateThread(nullptr, 0, function_impl, this, CREATE_SUSPENDED, nullptr);
	if (!win_handle) {
		cout << "CreateThread(\"" << name << "\")" << " has failed." << endl;
		while (true);
	}

	if (!SetThreadPriority(win_handle, THREAD_PRIORITY_LOWEST)) {
		cout << "SetThreadPriority(\"" << name << "\")" << " has failed." << endl;
		while (true);
	}
}

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
	[ ] Add windwows threads as the base
*/

class Scheduler {
	std::list<Thread*> m_ready { };
	std::set<Thread*> m_blocked { };
	Thread* m_currently_executing { nullptr };

	HANDLE idle_thread;

	unsigned m_time_slice_period = 4;
	unsigned m_tick_interval_ms = 1000;
public:
	Scheduler();
	void add_thread(Thread&);
	void run();
private:
	void add_to_ready(Thread&);
	void stop_currently_executing();
	void set_and_start_currently_executing(Thread& thread);
};

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

void a_handler(void)
{
	while (true) {
		//cout << "a running" << endl;
		//Sleep(200);
	}
}

void b_handler(void)
{
	while (true) {
		//cout << "b running" << endl;
		//Sleep(200);
	}
}

void c_handler(void)
{
	while (true) {
		//cout << "c running" << endl;
		//Sleep(200);
	}
}

bool windows_threading_init();

int main()
{
	if (!windows_threading_init()) {
		cout << "windows_threading_init() fail" << endl;
		return -1;
	}

	Thread a("a", a_handler, Thread::Priority::Medium);
	Thread b("b", b_handler, Thread::Priority::Medium);
	Thread c("c", c_handler, Thread::Priority::Low);

	Scheduler scheduler;
	scheduler.add_thread(a);
	scheduler.add_thread(b);
	scheduler.add_thread(c);

	scheduler.run();
}

// Force single core execution to use thread priorities to control thread execution
bool windows_threading_init()
{
	DWORD dwProcessAffinityMask = 1u;
	BOOL res = SetProcessAffinityMask(
		GetCurrentProcess(),
		dwProcessAffinityMask
	);

	if (!res) {
		cout << "SetProcessAffinityMask() failed" << endl;
		return false;
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
		return false;
	}

	return true;
}