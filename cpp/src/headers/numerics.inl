#ifndef CJM_NUMERICS_INL
#define CJM_NUMERICS_INL

namespace cjm::numerics
{
		
	template<typename Char, typename CharTraits>
	requires (cjm::numerics::concepts::char_or_wchar_t_with_traits<Char, CharTraits>)
		std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, compiler_used compiler)
	{
		using char_t = std::remove_const_t<Char>;
		if constexpr (std::is_same_v<char_t, char>)
		{
			os << get_text_narrow(compiler);
		}
		else
		{
			os << get_text_wide(compiler);
		}
		return os;
	}
}

namespace cjm::numerics::math_functions
{
	constexpr long double ten_to_the_power(int exponent)
	{
		if (exponent == 1)
		{
			return 10.0;
		}
		if (exponent == 0)
		{
			return 1.0;
		}
		if (exponent == -1)
		{
			return 0.1;
		}
		long double radix{ 10.0 };
		long double result = 1;
		while (exponent != 0)
		{
			if ((exponent & 1) == 1)
				result *= radix;
			exponent >>= 1;
			radix *= radix;
		}
		return result;
	}
	template<concepts::integer TInt>
	constexpr TInt int_pow(TInt radix, unsigned exponent)
	{
		if (radix == 0)
		{
			if (exponent == 0)
				throw std::domain_error("Zero cannot be raised to the zero power.");
			return radix;
		}
		if (radix == 1)
			return radix;
		if (exponent == 0)
		{
			return 1 * int_sign(radix);
		}
		if (exponent == 1)
		{
			return radix;
		}
		TInt result = 1;
		while (exponent != 0)
		{
			if ((exponent & 1) == 1)
				result *= radix;
			exponent >>= 1;
			radix *= radix;
		}
		return result;
	}

	template<typename TUInt>
	constexpr bool is_uint_power_of_two(TUInt testMe) noexcept
	{
		static_assert(std::numeric_limits<TUInt>::is_specialized &&
			std::numeric_limits<TUInt>::is_integer &&
			!std::numeric_limits<TUInt>::is_signed,
			"The number supplied must be an unsigned integer type.");
		return testMe > 0 && ((testMe & (testMe - 1)) == 0);
	}

	template<typename TInt>
	constexpr std::make_unsigned_t<TInt> int_abs(TInt val) noexcept
	{
		static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, "Function is only valid for non-boolean integers.");
		if constexpr (std::is_unsigned_v<TInt>)
			return val;
		else
			return static_cast<std::make_unsigned_t<TInt>>((val < 0 ? -val : val));
	}

	template<concepts::integer TInt>
	constexpr TInt int_sign(TInt val) noexcept
	{
		static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, "Function is only valid for non-boolean integers.");
		if constexpr (!std::numeric_limits<TInt>::is_signed)
			return TInt{ 1 };
		else
			return val < 0 ? TInt{ -1 } : TInt{ 1 };
	}

	template<concepts::unsigned_integer TUInt>
	constexpr TUInt int_lcm(TUInt first, TUInt second) noexcept
	{
		return std::lcm(first, second);
	}

	template <typename TInt>
	constexpr TInt floor_log2(TInt val)
	{
		if (val < 1)
			throw std::domain_error("Illegal attempt to find base-2 logarithm of zero or a negative number.");
		int rshiftCount = 0;
		while (val > 1)
		{
			val >>= 1;
			++rshiftCount;
		}
		return rshiftCount;
	}

	template<concepts::unsigned_integer TUInt>
	constexpr TUInt int_gcd(TUInt first, TUInt second) noexcept
	{
		return std::gcd(first, second);
	}

}

#ifdef __cpp_lib_bit_cast
template<typename To, typename From>
requires cjm::numerics::concepts::bit_castable<To, From>
CJM_BIT_CAST_CONST To cjm::numerics::bit_cast(const From& f) noexcept
{
	return std::bit_cast<To, From>(f);
}
#else
template<typename To, typename From>
requires cjm::numerics::concepts::bit_castable<To, From>
CJM_BIT_CAST_CONST To cjm::numerics::bit_cast(const From& f) noexcept
{
	//GCC seems to get all butt-hurt about private member variables even if type is trivial.
	//All c++ standard requires is trivially copyable and same size (and non-overlapping).  the concepts here
	//enforce MORE than that requirement (also require nothrow default constructible and alignment same)
	//Suppressing because uint128 is a type that is trivially copyable-to.
	auto dst = To{};
#if  (defined(__GNUC__) && !defined(__clang__))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
	std::memcpy(&dst, &f, sizeof(To));  /* no diagnostic for this one */
#pragma GCC diagnostic pop
#else
	std::memcpy(&dst, &f, sizeof(To));
#endif
	return dst;
}
#endif


#endif 
