#include "NativeThread.h"
#include <iostream>

using std::cout;
using std::endl;

void idle_thread()
{
	while (true);
}

NativeThreadController::NativeThreadController()
	: m_idle(NativeThread(static_cast<std::function<void()>>(idle_thread), IDLE_THREAD_PRIORITY))
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
}

void NativeThreadController::resume(NativeThread& thread)
{
	SetThreadPriority(thread.m_handle, THREAD_RUNNING_PRIORITY);
}

void NativeThreadController::pause(NativeThread& thread)
{
	SetThreadPriority(thread.m_handle, THREAD_PAUSED_PRIORITY);
}

static DWORD WINAPI function_impl(LPVOID lpParam)
{
	auto& function = *static_cast<std::function<void()>*>(lpParam);
	function();
	return 0;
}

NativeThread::NativeThread(std::function<void()> _function, int _priority)
	: function(_function)
	, m_handle(CreateThread(nullptr, 0, function_impl, static_cast<LPVOID>(&function), 0, nullptr))
{
	SetThreadPriority(m_handle, _priority);
}
