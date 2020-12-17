#include "uint128.hpp"


constexpr cjm::numerics::uint128 GetZero() noexcept
{
	constexpr cjm::numerics::uint128 ret{};
	static_assert(ret == 0);
	return ret;
}

void cjm::numerics::uint128::instrumented_div_mod(std::basic_ostream<char>& stream, uint128 dividend, uint128 divisor,
	uint128* quotient_ret, uint128* remainder_ret)
{
	constexpr auto newl = "\n";
	if (divisor == 0)
	{
		throw std::domain_error("Division and/or modulus by zero is forbidden.");
	}
	if (divisor > dividend)
	{
		*quotient_ret = 0;
		*remainder_ret = dividend;
		return;
	}
	if (divisor == dividend)
	{
		*quotient_ret = 1;
		*remainder_ret = 0;
		return;
	}

	stream << std::hex;
	stream << "BEGIN INSTRUMENTED DIV MOD 128" << newl;
	stream << "dividend: 0x" << dividend << newl;
	stream << "divisor: 0x" << divisor << newl;

	auto denominator = divisor;
	auto quotient = MakeUint128(0, 0);

	stream << "denominator: 0x" << denominator << newl;
	stream << "quotient: 0x" << quotient << newl;

	const auto flsDividend = fls(dividend);
	const auto flsDenominator = fls(denominator);
	const auto shiftAmount = flsDividend - flsDenominator;
	denominator <<= shiftAmount;
	stream << std::dec;
	stream << "DECIMAL FLS RESULTS:" << newl;
	stream << "flsDividend: " << flsDividend << newl;
	stream << "flsDenominator: " << flsDenominator << newl;
	stream << "shiftAmount: " << shiftAmount << newl;

	stream << std::hex;
	stream << "HEX FLS RESULTS:" << newl;
	stream << "flsDividend: 0x" << flsDividend << newl;
	stream << "flsDenominator: 0x" << flsDenominator << newl;
	stream << "shiftAmount: 0x" << shiftAmount << newl;

	stream << "denominator <<= 0x" << shiftAmount << " == 0x" << denominator << newl;
	stream << std::boolalpha;
	stream << newl << newl << "STARTING CALCULATION LOOP" << newl;
	for (int i = 0; i <= shiftAmount; i++)
	{
		stream << "i = " << std::dec << i << " i <= " << shiftAmount 
			<< "= " << (i <= shiftAmount) << std::hex << newl;
		stream << "quotient: 0x" << quotient << newl;
		quotient <<= 1;
		stream << "(quotient <<= 1): 0x" << quotient << newl;
		stream << "denominator: 0x" << denominator << newl;
		bool dividendGreaterThanOrEqualToDenom = dividend >= denominator;
		stream << "dividendGreaterThanOrEqualToDenom: " << dividendGreaterThanOrEqualToDenom << newl;
		if (dividendGreaterThanOrEqualToDenom)
		{
			dividend -= denominator;
			quotient |= 1;
			stream << "dividend -= denominator: 0x" << dividend << newl;
			stream << "quotient |= 1: 0x" << quotient << newl;
		}
		denominator >>= 1;
		stream << "denominator >>= 1: 0x" << denominator << newl;
	}

	stream << "final quotient: 0x" << quotient << newl;
	stream << "final remainder (dividend): 0x" << dividend << newl;
	*quotient_ret = quotient;
	*remainder_ret = dividend;
	stream << "END INSTRUMENTED DIV MOD 128" << newl;
} 

cjm::numerics::uint128::operator float() const 
{
	return static_cast<float>(m_low) + std::ldexp(static_cast<float>(m_high), 64);
}

cjm::numerics::uint128::operator double() const 
{
	return static_cast<double>(m_low) + std::ldexp(static_cast<double>(m_high), 64);
}

cjm::numerics::uint128::operator long double() const 
{
	return static_cast<long double>(m_low) +
		std::ldexp(static_cast<long double>(m_high), 64);
}
#ifdef CJM_HAVE_BUILTIN_128
cjm::numerics::uint128& cjm::numerics::uint128::operator=(__uint128_t other) noexcept
{
    m_low = static_cast<std::uint64_t>(other);
    other >>= 64;
    m_high = static_cast<std::uint64_t>(other);
    return *this;
}


cjm::numerics::uint128::operator unsigned __int128() const noexcept
{
    unsigned __int128 ret = m_high;
    ret <<= 64;
    ret |= m_low;
    return ret;
}

cjm::numerics::uint128::uint128(__uint128_t other) noexcept
{
    m_low = static_cast<std::uint64_t>(other);
    other >>= 64;
    m_high = static_cast<std::uint64_t>(other);
}

#endif