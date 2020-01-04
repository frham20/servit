#include "socket.h"
#include "..\thread\thread_pool.h"

namespace servit
{
	bool socket::create( protocol proto )
	{
		if ( !get_imp().create( proto ) )
			return false;

		return get_io_thread_pool()->register_io_object( this );
	}

	void socket::close()
	{
		get_io_thread_pool()->unregister_io_object( this );
		get_imp().close();
	}
}