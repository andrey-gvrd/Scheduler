#pragma once

#include <string>
#include <functional>
#include <windows.h>

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

	unsigned executing_for{ 0 };

	Thread(std::string, std::function<void()>, Priority);

	bool operator<(const Thread& rhs) { return static_cast<int>(priority) > static_cast<int>(rhs.priority); }
	bool operator>(const Thread& rhs) { return rhs.priority < priority; }
};
