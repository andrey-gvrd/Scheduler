#include <iostream>
#include <string>
#include "Scheduler.h"
#include "Thread.h"

using std::cout;
using std::endl;

class CustomThreadA : public Thread {
	using Thread::Thread;
public:
	void handler() override {
		while (true) {
			//cout << m_name << " running" << endl;
			sleep_ticks(2);
		}
	}
};

class CustomThreadB : public Thread {
	using Thread::Thread;
public:
	void handler() override {
		while (true) {
			//cout << m_name << " running" << endl;
		}
	}
};

int main()
{
	Scheduler scheduler;
	CustomThreadA thread_a_1("thread_a_1");
	CustomThreadA thread_a_2("thread_a_2");
	CustomThreadB thread_b_1("thread_b_1");
	CustomThreadB thread_b_2("thread_b_2", Thread::Priority::Low);
	scheduler.add_thread(thread_a_1);
	scheduler.add_thread(thread_a_2);
	scheduler.add_thread(thread_b_1);
	scheduler.add_thread(thread_b_2);

	scheduler.run();
}
