#pragma once

namespace servit
{
	class thread_pool;

	class async_service_imp
	{
	public:
		explicit async_service_imp( thread_pool* pool );

	private:
		thread_pool* m_pool = nullptr;
	};
}
