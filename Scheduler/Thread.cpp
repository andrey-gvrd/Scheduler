#include "Thread.h"
#include <iostream>

DWORD WINAPI function_impl(LPVOID lpParam) {
	static_cast<Thread*>(lpParam)->function();
	return 0;
}

using std::cout;
using std::endl;

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