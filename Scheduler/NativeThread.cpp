#include "NativeThread.h"

void idle_thread()
{
	while (true);
}

NativeThreadController::NativeThreadController()
	: m_idle(NativeThread(static_cast<std::function<void()>>(idle_thread), IDLE_THREAD_PRIORITY))
{
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
	static_cast<NativeThread*>(lpParam)->function();
	return 0;
}

NativeThread::NativeThread(std::function<void()> _function, int priority)
	: function(_function)
{
	m_handle = CreateThread(nullptr, 0, function_impl, this, 0, nullptr);
	SetThreadPriority(m_handle, priority);
}
