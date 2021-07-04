#include "Thread.h"
#include <iostream>

using std::cout;
using std::endl;

static void handler_impl(void* context)
{
	auto object = static_cast<Thread*>(context);
	object->handler();
}

Thread::Thread(std::string _name, Priority _priority)
	: m_name(_name)
	, m_priority(_priority)
	, m_state(Thread::State::Ready)
	, m_native_thread(NativeThread(std::bind(handler_impl, this), THREAD_PAUSED_PRIORITY, m_name))
{
}

void Thread::sleep_ms(MilliSeconds ms)
{

}

void Thread::sleep_ticks(Ticks ticks)
{
	if (!ticks) {
		return;
	}

	m_sleep_for = ticks;
	yield();
}

void Thread::yield()
{
	m_native_thread.pause();
}