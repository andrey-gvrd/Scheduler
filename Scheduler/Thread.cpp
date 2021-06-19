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
	: name(_name)
	, priority(_priority)
	, state(Thread::State::Ready)
	, native_thread(NativeThread(std::bind(handler_impl, this)))
{
}

void Thread::sleep_ms(unsigned ms)
{

}