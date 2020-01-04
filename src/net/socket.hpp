namespace servit
{
	inline void socket::close()
	{
		get_imp().close();
	}

	inline bool socket::bind( unsigned short port )
	{
		return get_imp().bind( port );
	}

	inline socket::~socket()
	{
		close();
	}

	inline bool socket::create( protocol proto )
	{
		return get_imp().create( proto );
	}

	inline bool tcp_socket::create()
	{
		return socket::create( protocol::tcp );
	}

	inline bool tcp_socket::listen()
	{
		return get_imp().listen();
	}
}