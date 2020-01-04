#pragma once

namespace servit
{
	template<typename Imp>
	class imp : protected Imp
	{
	public:
		Imp&       get_imp();
		const Imp& get_imp() const;
	};

	template<typename Imp>
	inline Imp& imp<Imp>::get_imp()
	{
		return *this;
	}

	template<typename Imp>
	inline const Imp& imp<Imp>::get_imp() const
	{
		return *this;
	}
}
