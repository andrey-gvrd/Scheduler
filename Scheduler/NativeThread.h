#pragma once

#include <functional>
#include <windows.h>

static const int IDLE_THREAD_PRIORITY    = THREAD_PRIORITY_NORMAL;
static const int THREAD_PAUSED_PRIORITY  = THREAD_PRIORITY_BELOW_NORMAL;
static const int THREAD_RUNNING_PRIORITY = THREAD_PRIORITY_ABOVE_NORMAL;
static const int SCHEDULER_PRIORITY      = THREAD_PRIORITY_HIGHEST;

class NativeThread
{
	HANDLE m_handle;
public:
	std::function<void()> function;
	NativeThread(std::function<void()>, int m_priority = THREAD_PAUSED_PRIORITY);

	friend class NativeThreadController;
};

class NativeThreadController
{
	NativeThread m_idle;
public:
	NativeThreadController();
	void resume(NativeThread&);
	void pause(NativeThread&);
};
