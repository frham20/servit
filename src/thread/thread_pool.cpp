#include "thread_pool.h"

static servit::thread_pool* s_io_thread_pool = nullptr;

namespace servit
{
	void set_io_thread_pool( thread_pool* pool )
	{
		s_io_thread_pool = pool;
	}

	thread_pool* get_io_thread_pool()
	{
		return s_io_thread_pool;
	}
}