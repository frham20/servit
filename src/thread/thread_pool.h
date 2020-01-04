#pragma once
#include "..\core\imp.h"
#include "..\platforms\win\thread_pool_imp.h"

namespace servit
{
	typedef void ( *io_callback )( bool success, int bytes_processed );

	class thread_pool : public imp<thread_pool_imp>
	{
	public:
		static constexpr size_t DEFAULT_MAX_THREADS = 0;

	public:
		thread_pool() = default;
		explicit thread_pool( size_t max_threads );
		~thread_pool();

		bool create( size_t max_threads = DEFAULT_MAX_THREADS );
		void close();

		bool register_io_object( socket* s );
		bool unregister_io_object( socket* s );
	};

	void         set_io_thread_pool( thread_pool* pool );
	thread_pool* get_io_thread_pool();

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

	inline void thread_pool::close()
	{
		return get_imp().close();
	}

	inline bool thread_pool::register_io_object( socket* s )
	{
		return get_imp().register_io_object( s );
	}

	inline bool thread_pool::unregister_io_object( socket* s )
	{
		return get_imp().unregister_io_object( s );
	}
}