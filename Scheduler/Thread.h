#pragma once

#include <string>
#include <functional>
#include "NativeThread.h"

class Thread {
	friend class Scheduler;

public:
	enum class State {
		Ready,
		Blocked
	};
	enum class Priority {
		Low = 0,
		Medium = 1,
		High = 2
	};

	Thread(std::string _name = "", Priority _priority = Priority::Medium);
	virtual void handler() = 0;
	void sleep_ms(unsigned);

private:
	Priority priority;
	State state;
	NativeThread native_thread;
	std::string name;

	unsigned executing_for{ 0 };

	bool operator<(const Thread& rhs) { return static_cast<int>(priority) > static_cast<int>(rhs.priority); }
	bool operator>(const Thread& rhs) { return rhs.priority < priority; }
};
