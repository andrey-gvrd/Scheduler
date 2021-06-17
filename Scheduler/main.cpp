#include <iostream>
#include <string>
#include "Scheduler.h"
#include "Thread.h"

using std::cout;
using std::endl;

void a_handler(void)
{
	while (true) {
		//cout << "a running" << endl;
		//Sleep(200);
	}
}

void b_handler(void)
{
	while (true) {
		//cout << "b running" << endl;
		//Sleep(200);
	}
}

void c_handler(void)
{
	while (true) {
		//cout << "c running" << endl;
		//Sleep(200);
	}
}

int main()
{
	Thread a("a", a_handler, Thread::Priority::Medium);
	Thread b("b", b_handler, Thread::Priority::Medium);
	Thread c("c", c_handler, Thread::Priority::Low);

	Scheduler scheduler;
	scheduler.add_thread(a);
	scheduler.add_thread(b);
	scheduler.add_thread(c);

	scheduler.run();
}
