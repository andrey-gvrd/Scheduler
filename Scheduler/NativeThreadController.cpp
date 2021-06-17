#include "NativeThreadController.h"

void idle_thread()
{
	while (true);
}

NativeThreadController::NativeThreadController()
	: m_idle(NativeThread(static_cast<std::function<void()>>(idle_thread), THREAD_PRIORITY_NORMAL))
{
}

void NativeThreadController::resume(NativeThread& thread)
{
	SetThreadPriority(thread.m_handle, THREAD_PRIORITY_ABOVE_NORMAL);
}

void NativeThreadController::pause(NativeThread& thread)
{
	SetThreadPriority(thread.m_handle, THREAD_PRIORITY_BELOW_NORMAL);
}
