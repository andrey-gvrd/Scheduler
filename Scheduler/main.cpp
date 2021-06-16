#include <iostream>
#include <string>
#include <windows.h>
#include "Scheduler.h"

using std::cout;
using std::endl;

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