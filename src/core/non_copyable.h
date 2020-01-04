#pragma once

namespace servit
{
	class non_copyable
	{
	protected:
		non_copyable()  = default;
		~non_copyable() = default;

	private:
		non_copyable( const non_copyable& ) = delete;
		non_copyable( non_copyable&& )      = delete;
		non_copyable& operator=( const non_copyable& ) = delete;
		non_copyable& operator=( non_copyable&& ) = delete;
	};
}