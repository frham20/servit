#include "thread_pool_imp.h"
#include "..\..\net\socket.h"

namespace servit
{
	static VOID CALLBACK SocketIoCompletionCallback( _Inout_ PTP_CALLBACK_INSTANCE Instance, _Inout_opt_ PVOID Context,
	                                                 _Inout_opt_ PVOID Overlapped, _In_ ULONG IoResult,
	                                                 _In_ ULONG_PTR NumberOfBytesTransferred, _Inout_ PTP_IO Io )
	{
	}

	thread_pool_imp::thread_pool_imp()
	{
		::InitializeThreadpoolEnvironment( &m_env );
	}

	thread_pool_imp::~thread_pool_imp()
	{
		close();
		::DestroyThreadpoolEnvironment( &m_env );
	}

	bool thread_pool_imp::create( size_t max_threads )
	{
		if ( m_handle != nullptr )
			return false;

		m_handle = ::CreateThreadpool( nullptr );
		::SetThreadpoolCallbackPool( &m_env, m_handle );

		if ( max_threads > 0 )
			::SetThreadpoolThreadMaximum( m_handle, static_cast<DWORD>( max_threads ) );

		return true;
	}

	bool thread_pool_imp::register_io_object( socket* s )
	{
		HANDLE socket_handle = reinterpret_cast<HANDLE>( s->get_imp().get_handle() );
		PTP_IO io            = ::CreateThreadpoolIo( socket_handle, SocketIoCompletionCallback, s, &m_env );
		if ( io == nullptr )
			return false;

		s->get_imp().set_io_handle( io );

		return true;
	}

	bool thread_pool_imp::unregister_io_object( socket* s )
	{
		// TODO
	}

	void thread_pool_imp::close()
	{
		if ( m_handle != nullptr )
		{
			::CloseThreadpool( m_handle );
			m_handle = nullptr;
		}
	}

	bool thread_pool_imp::is_valid() const
	{
		return m_handle != nullptr;
	}
}
