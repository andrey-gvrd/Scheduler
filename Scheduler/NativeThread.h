#pragma once

#include <functional>
#include <windows.h>

class NativeThread
{
	HANDLE m_handle;
public:
	std::function<void()> function;
	NativeThread(std::function<void()>, int priority = THREAD_PRIORITY_BELOW_NORMAL);

	friend class NativeThreadController;
};

