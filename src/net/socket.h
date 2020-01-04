#pragma once
#include "..\core\imp.h"
#include "..\platforms\win\socket_imp.h"

namespace servit
{
	class async;
	class async_service;

	enum class protocol
	{
		tcp,
		udp
	};

	class socket : public imp<socket_imp>
	{
	public:
		void close();

		// void read();
		/*
		template<typename Callback>
		void read_async( void* buffer, size_t len, Callback callback );
		*/

		bool bind( unsigned short port );

	protected:
		socket() = default;
		~socket();

		bool create( protocol proto );
	};

	typedef void ( *accept_callback )();

	class tcp_socket : public socket
	{
	public:
		tcp_socket()  = default;
		~tcp_socket() = default;
		bool create();
		bool listen();
		// tcp_socket accept();
		void accept_async();
	};

	/*
	template<typename Imp>
	class p_udp_socket : public p_socket<Imp>
	{
	public:
	    p_udp_socket() = default;
	    ~p_udp_socket() = default;
	    bool create();
	};
	*/

	inline bool socket::bind( unsigned short port )
	{
		return get_imp().bind( port );
	}

	inline socket::~socket()
	{
		close();
	}

	/*
	template<typename Callback>
	inline void socket::read_async( void* buffer, size_t size, Callback callback )
	{
	    get_imp().read_async( buffer, size, callback );
	}
	*/
	inline bool tcp_socket::create()
	{
		return socket::create( protocol::tcp );
	}

	inline bool tcp_socket::listen()
	{
		return get_imp().listen();
	}
}
