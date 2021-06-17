#include "NativeThread.h"
#include <iostream>

using std::cout;
using std::endl;

static DWORD WINAPI function_impl(LPVOID lpParam)
{
	static_cast<NativeThread*>(lpParam)->function();
	return 0;
}

NativeThread::NativeThread(std::function<void()> _function, int priority)
	: function(_function)
{
	m_handle = CreateThread(nullptr, 0, function_impl, this, 0, nullptr);
	SetThreadPriority(m_handle, priority);
}
