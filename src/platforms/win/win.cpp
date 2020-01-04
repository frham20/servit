#include "win.h"

static WSADATA s_wsadata = {0};

namespace servit
{
	bool platform_init()
	{
		if (::WSAStartup( MAKEWORD( 2, 2 ), &s_wsadata ) != 0 )
			return false;

		return true;
	}

	void platform_close()
	{
		::WSACleanup();
	}
}