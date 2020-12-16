#ifndef CJM_FLOATCOMPARER_HPP
#define CJM_FLOATCOMPARER_HPP
#include "numerics.hpp"
namespace cjm::numerics
{

	template<typename TFloat, int Exponent>
	struct float_comparer
	{
		static_assert(Exponent < 0);
		static_assert(std::is_same_v<TFloat, float> || std::is_same_v<TFloat, double> || std::is_same_v<TFloat, long double>);
		static constexpr int exponent{ Exponent };
		static constexpr TFloat get_epsilon();
		static constexpr TFloat epsilon{ get_epsilon() };

		static constexpr int compare(TFloat lhs, TFloat rhs);
	};

	template <typename TFloat, int Exponent>
	constexpr TFloat float_comparer<TFloat, Exponent>::get_epsilon()
	{
		return math_functions::ten_to_the_power(exponent);
	}

	template <typename TFloat, int Exponent>
	constexpr int float_comparer<TFloat, Exponent>::compare(TFloat lhs, TFloat rhs)
	{
		TFloat difference = lhs - rhs;
		TFloat absDifference = difference < 0 ? -difference : difference;
		if (absDifference < epsilon)
			return 0;
		if (lhs > rhs)
			return 1;
		return -1;
	}
}
#endif