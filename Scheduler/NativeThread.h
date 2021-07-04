#pragma once

#include <functional>
#include <string>
#include <windows.h>

static const int IDLE_THREAD_PRIORITY    = THREAD_PRIORITY_NORMAL;
static const int THREAD_PAUSED_PRIORITY  = THREAD_PRIORITY_BELOW_NORMAL;
static const int THREAD_RUNNING_PRIORITY = THREAD_PRIORITY_ABOVE_NORMAL;
static const int SCHEDULER_PRIORITY      = THREAD_PRIORITY_HIGHEST;

class NativeThread
{
	HANDLE m_handle;
	std::string m_name;
public:
	std::function<void()> function;
	NativeThread(std::function<void()>, int _priority = THREAD_PAUSED_PRIORITY, std::string _name = "");
	void pause();
	void resume();
};
