#pragma once
#include "..\..\core\non_copyable.h"
#include "win.h"

namespace servit
{
	class socket;

	class thread_pool_imp : private non_copyable
	{
	public:
		thread_pool_imp();
		~thread_pool_imp();

		bool create( size_t max_threads );
		void close();
		bool is_valid() const;

		bool register_io_object( socket* s );
		bool unregister_io_object( socket* s );

		TP_CALLBACK_ENVIRON&       get_env();
		const TP_CALLBACK_ENVIRON& get_env() const;

	private:
		TP_CALLBACK_ENVIRON m_env    = {0};
		PTP_POOL            m_handle = nullptr;
	};

	inline TP_CALLBACK_ENVIRON& thread_pool_imp::get_env()
	{
		return m_env;
	}

	inline const TP_CALLBACK_ENVIRON& thread_pool_imp::get_env() const
	{
		return m_env;
	}
}
