#pragma once
#include "..\core\imp.h"
#include "..\net\socket.h"
#include "async.h"
#include <vector>

#include "..\platforms\win\async_service_imp.h"

namespace servit
{
	typedef unsigned int async_handle;

	class async_service : protected imp<async_service_imp>
	{
	public:
		explicit async_service( thread_pool* pool );
		~async_service() = default;

		async_handle register_async( socket asocket );
		void         unregister_async( async_handle handle );

		bool queue( async_handle handle );
		bool cancel( async_handle handle );
	};
}