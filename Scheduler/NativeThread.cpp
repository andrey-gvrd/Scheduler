#include "NativeThread.h"
#include <iostream>

using std::cout;
using std::endl;

void NativeThread::resume()
{
	SetThreadPriority(m_handle, THREAD_RUNNING_PRIORITY);
}

void NativeThread::pause()
{
	SetThreadPriority(m_handle, THREAD_PAUSED_PRIORITY);
}

static DWORD WINAPI function_impl(LPVOID lpParam)
{
	auto& function = *static_cast<std::function<void()>*>(lpParam);
	function();
	return 0;
}

NativeThread::NativeThread(std::function<void()> _function, int _priority, std::string _name)
	: function(_function)
	, m_name(_name)
	, m_handle(CreateThread(nullptr, 0, function_impl, static_cast<LPVOID>(&function), CREATE_SUSPENDED, nullptr))
{
	SetThreadPriority(m_handle, _priority);

	auto _wname = std::wstring(m_name.begin(), m_name.end());
	SetThreadDescription(m_handle, _wname.c_str());

	ResumeThread(m_handle);
}
