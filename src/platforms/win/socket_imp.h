#pragma once
#include "..\..\core\non_copyable.h"
#include "win.h"

namespace servit
{
	enum class protocol;
	class socket;

	class socket_imp
	{
	public:
		socket_imp()                          = default;
		socket_imp( const socket_imp& other ) = delete;
		socket_imp( socket_imp&& other );
		~socket_imp();

		socket_imp& operator=( socket_imp& other );

		bool create( protocol proto );
		void close();

		bool bind( unsigned short port );
		bool listen();

		template<typename Callback>
		bool read_async( void* buffer, size_t len, Callback callback );

		SOCKET get_handle() const;

		void   set_io_handle( PTP_IO handle );
		PTP_IO get_io_handle() const;

	private:
		SOCKET     m_handle     = INVALID_SOCKET;
		OVERLAPPED m_overlapped = {0};
		PTP_IO     m_io_handle  = nullptr;
	};

	inline SOCKET socket_imp::get_handle() const
	{
		return m_handle;
	}

	inline void socket_imp::set_io_handle( PTP_IO handle )
	{
		m_io_handle = handle;
	}

	inline PTP_IO socket_imp::get_io_handle() const
	{
		return m_io_handle;
	}

	template<typename Callback>
	bool socket_imp::read_async( void* buffer, size_t size, Callback callback )
	{
		WSABUF recvBuffer = {0};
		recvBuffer.buf    = buffer;
		recvBuffer.len    = size;

		DWORD receiveFlags = 0;

		::StartThreadpoolIo( m_io_handle );
		if (::WSARecv( m_handle, &recvBuffer, 1, nullptr, &receiveFlags, &m_overlapped, nullptr ) == SOCKET_ERROR )
		{
			const int wsaError = ::WSAGetLastError();
			if ( wsaError != WSA_IO_PENDING )
			{
				::CancelThreadpoolIo( m_io_handle );
				// need to call callback?
				return false;
			}
		}

		return true;
	}
}