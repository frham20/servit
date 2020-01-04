#pragma once

namespace servit
{
	class async_imp
	{
	public:
		PTP_IO get_async_handle() const;

	private:
		PTP_IO m_handle;
	};
}
