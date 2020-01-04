#pragma once
#include "socket.h"

namespace servit
{
	class server : private non_copyable
	{
	public:
		server()  = default;
		~server() = default;

	private:
		// socket m_listen_socket;
	};
}