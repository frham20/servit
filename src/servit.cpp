#include "servit.h"
#include "platforms/win/win.h"

namespace servit
{
	bool init()
	{
		return platform_init();
	}

	void close()
	{
		platform_close();
	}
}