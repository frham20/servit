#pragma once
#include <sdkddkver.h>

#include <winsock2.h>

#include <MSWSock.h>
#include <WS2tcpip.h>
#include <threadpoolapiset.h>

namespace servit
{
	bool platform_init();
	void platform_close();
}

#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "mswsock.lib" )
