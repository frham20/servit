#pragma once

#include "net\server.h"
#include "net\socket.h"
#include "thread\thread_pool.h"

namespace servit
{
	bool init();
	void close();
}

#pragma comment( lib, "servit.lib" )
