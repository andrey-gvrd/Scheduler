#pragma once

#include "NativeThread.h"

class NativeThreadController
{
	NativeThread m_idle;
public:
	NativeThreadController();
	void resume(NativeThread&);
	void pause(NativeThread&);
};
