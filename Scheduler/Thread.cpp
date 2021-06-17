#include "Thread.h"
#include <iostream>

using std::cout;
using std::endl;

Thread::Thread(std::string _name, std::function<void()> _function, Thread::Priority _priority)
	: name(_name)
	, priority(_priority)
	, state(Thread::State::Ready)
	, thread(NativeThread(_function))
{
}