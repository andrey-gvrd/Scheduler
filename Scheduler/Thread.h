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
	using Ticks = unsigned;
	using MilliSeconds = unsigned;

	Thread(std::string _name = "", Priority _priority = Priority::Medium);
	virtual void handler() = 0;
	void sleep_ms(MilliSeconds);
	void sleep_ticks(Ticks);

protected:
	std::string m_name;

private:
	Priority m_priority;
	State m_state;
	NativeThread m_native_thread;

	Ticks m_executing_for{ 0 };
	Ticks m_sleep_for{ 0 };

	bool operator<(const Thread& rhs) { return static_cast<int>(m_priority) > static_cast<int>(rhs.m_priority); }
	bool operator>(const Thread& rhs) { return rhs.m_priority < m_priority; }

	void yield();
};
