namespace servit
{
	inline thread_pool::thread_pool( size_t max_threads )
	{
		create( max_threads );
	}

	inline thread_pool::~thread_pool()
	{
		close();
	}

	inline bool thread_pool::create( size_t max_threads )
	{
		return get_imp().create( max_threads );
	}

	inline void thread_pool::wait_for_pending_jobs()
	{
	}

	inline void thread_pool::close()
	{
		return get_imp().close();
	}
}
