#include "socket_imp.h"
#include "..\..\net\socket.h"
#include <memory>

namespace servit
{
	socket_imp::~socket_imp()
	{
	}

	socket_imp::socket_imp( socket_imp&& other )
	    : m_handle( std::move( other.m_handle ) )
	    , m_io_handle( std::move( other.m_io_handle ) )
	{
		other.m_handle    = INVALID_SOCKET;
		other.m_io_handle = nullptr;
	}

	socket_imp& socket_imp::operator=( socket_imp& other )
	{
		m_handle    = std::move( other.m_handle );
		m_io_handle = std::move( other.m_io_handle );

		other.m_handle    = INVALID_SOCKET;
		other.m_io_handle = nullptr;

		return *this;
	}

	bool socket_imp::create( protocol proto )
	{
		int sock_type  = SOCK_STREAM;
		int sock_proto = IPPROTO_TCP;

		if ( proto == protocol::udp )
		{
			sock_type = SOCK_DGRAM;
			sock_type = IPPROTO_UDP;
		}

		SOCKET handle = ::WSASocketW( AF_INET, sock_type, sock_proto, nullptr, 0, WSA_FLAG_OVERLAPPED );
		if ( handle == INVALID_SOCKET )
			return false;

		m_handle = handle;

		return true;
	}

	bool socket_imp::bind( unsigned short port )
	{
		wchar_t str_port[6] = {0};
		if ( _itow_s( port, str_port, 10 ) != 0 )
			return false;

		int sock_type     = SOCK_STREAM;
		int sock_type_len = sizeof( sock_type );
		::getsockopt( m_handle, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>( &sock_type ), &sock_type_len );

		ADDRINFOW ainfo   = {0};
		ainfo.ai_family   = AF_INET;
		ainfo.ai_socktype = sock_type;
		ainfo.ai_protocol = sock_type == SOCK_STREAM ? IPPROTO_TCP : IPPROTO_UDP;
		ainfo.ai_flags    = AI_PASSIVE;

		ADDRINFOW* result = nullptr;
		if (::GetAddrInfoW( nullptr, str_port, &ainfo, &result ) != 0 )
			return false;

		if (::bind( m_handle, result->ai_addr, static_cast<int>( result->ai_addrlen ) ) != 0 )
		{
			::FreeAddrInfoW( result );
			return false;
		}

		::FreeAddrInfoW( result );
		return true;
	}

	bool socket_imp::listen()
	{
		return ::listen( m_handle, SOMAXCONN ) == 0;
	}

	void socket_imp::close()
	{
		if ( m_handle != INVALID_SOCKET )
		{
			::closesocket( m_handle );
			m_handle = INVALID_SOCKET;
		}
	}
}