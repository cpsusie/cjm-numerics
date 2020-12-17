#ifndef CJM_NUMERICS_INL
#define CJM_NUMERICS_INL



/* FLS64 IMPROVEMENTS
template <typename T>
constexpr inline void step(T& n, int& pos, int shift) noexcept
{
    if (n >= (static_cast<T>(1) << shift))
    {
        n = n >> shift;
        pos |= shift;
    }   
}

// Returns the 0-based position of the last set bit (i.e., most significant bit)
// in the given uint64_t. The argument may not be 0.
//
// For example:
//   Given: 5 (decimal) == 101 (binary)
//   Returns: 2
#define STEP(T, n, pos, sh)           \
    do {                                        \
    if ((n) >= (static_cast<T>(1) << (sh))) { \
        (n) = (n) >> (sh);                      \
        (pos) |= (sh);                          \
    }                                         \
    } while (0)
constexpr inline int fls64_v1(std::uint64_t n) noexcept
{
    assert(n != 0);
    int pos = 0;
    STEP(uint64_t, n, pos, 0x20);
    auto n32 = static_cast<std::uint32_t>(n);
    STEP(std::uint32_t, n32, pos, 0x10);
    STEP(std::uint32_t, n32, pos, 0x08);
    STEP(std::uint32_t, n32, pos, 0x04);
    return pos + ((std::uint64_t{ 0x3333333322221100 } >> (n32 << 2)) & 0x3);
}
#undef STEP

constexpr inline int fls64_v2(std::uint64_t n) noexcept
{
    assert(n != 0);
    int pos = 0;
    step(n, pos, 0x20);
    auto n32 = static_cast<std::uint32_t>(n);
    step(n32, pos, 0x10);
    step(n32, pos, 0x08);
    step(n32, pos, 0x04);
    return pos + ((std::uint64_t{ 0x3333333322221100 } >> (n32 << 2)) & 0x3);
}

*/
constexpr long double cjm::numerics::math_functions::ten_to_the_power(int exponent)
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
template<typename TInt>
constexpr TInt cjm::numerics::math_functions::int_pow(TInt radix, unsigned exponent)
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
constexpr bool cjm::numerics::math_functions::is_uint_power_of_two(TUInt testMe) noexcept
{
	static_assert(std::numeric_limits<TUInt>::is_specialized && 
		std::numeric_limits<TUInt>::is_integer && 
		!std::numeric_limits<TUInt>::is_signed, 
		"The number supplied must be an unsigned integer type.");
	return testMe > 0 && ((testMe & (testMe - 1)) == 0);
}

template<typename TInt>
constexpr std::make_unsigned_t<TInt> cjm::numerics::math_functions::int_abs(TInt val) noexcept
{
	static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, "Function is only valid for non-boolean integers.");
	if constexpr (std::is_unsigned_v<TInt>)
		return val;
	else
		return static_cast<std::make_unsigned_t<TInt>>((val < 0 ? -val : val));
}

template<typename TInt>
constexpr TInt cjm::numerics::math_functions::int_sign(TInt val) noexcept
{
	static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, "Function is only valid for non-boolean integers.");
	if constexpr (std::is_unsigned_v<TInt>)
		return TInt{ 1 };
	else
		return val < 0 ? TInt{ -1 } : TInt{ 1 };
}

template<typename TInt>
constexpr TInt cjm::numerics::math_functions::int_lcm(TInt first, TInt second) 
{
	return std::lcm(first, second);
}

template <typename TInt>
constexpr TInt cjm::numerics::math_functions::floor_log2(TInt val)
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

template<typename TInt>
constexpr TInt cjm::numerics::math_functions::int_gcd(TInt first, TInt second) noexcept
{
	return std::gcd(first, second);
}


#endif 
