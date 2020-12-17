#ifndef CJM_NUMERICS_HPP
#define CJM_NUMERICS_HPP
#include <cmath>
#include <limits>
#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <boost/functional/hash.hpp>
#include <numeric>

namespace cjm
{
	namespace numerics
	{
	    constexpr bool has_intrinsic_u128 =
#ifdef __SIZEOF_INT128__
        true;
#ifndef CJM_HAVE_BUILTIN_128
#define CJM_HAVE_BUILTIN_128
#endif
#else
	    false;
#ifdef CJM_HAVE_BUILTIN_128
#undef CJM_HAVE_BUILTIN_128
#endif
#endif
	    constexpr size_t uint128_align = has_intrinsic_u128 ? alignof(unsigned __int128) : alignof(std::uint64_t);



		class uint128;
				
		using uint128_alt = boost::multiprecision::uint128_t;
		using int128 = boost::multiprecision::int128_t;
		using uint256 = boost::multiprecision::uint256_t;
		using int256 = boost::multiprecision::int256_t;
		using huge_decimal = boost::multiprecision::cpp_dec_float_100;
		using large_decimal = boost::multiprecision::cpp_dec_float_50;
		using norm_decimal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<28>>;

		namespace math_functions
		{
			template<typename TInt>
			constexpr std::make_unsigned_t<TInt> int_abs(TInt val) noexcept;

			template<typename TInt>
			constexpr TInt int_sign(TInt val) noexcept;

			template<typename TInt>
			constexpr TInt int_gcd(TInt first, TInt second) noexcept;

			template<typename TInt>
			constexpr TInt int_lcm(TInt first, TInt second);
			

			template<typename TInt>
			constexpr TInt floor_log2(TInt val);

			template<typename TInt>
			constexpr TInt int_pow(TInt radix, unsigned exponent);

			template<typename TUInt>
			constexpr bool is_uint_power_of_two(TUInt testMe) noexcept;

			constexpr long double ten_to_the_power(int exponent);
			
			
		}
	}
}



namespace std
{
	template<>
	struct hash<cjm::numerics::uint128>;

	template<>
	class numeric_limits<cjm::numerics::uint128>;

	template <>
	struct is_arithmetic <cjm::numerics::uint128>;

	template <> struct is_integral<cjm::numerics::uint128>;

	template <> struct is_unsigned<cjm::numerics::uint128>;

	template<> struct is_signed<cjm::numerics::uint128>;

	template<>
	struct is_arithmetic<cjm::numerics::uint128_alt> : std::true_type {	};

	template<>
	struct is_integral<cjm::numerics::uint128_alt> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint128_alt>::is_integer,
		std::true_type, std::false_type> {};


	template<>
	struct is_unsigned<cjm::numerics::uint128_alt> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint128_alt>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::uint128_alt> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint128_alt>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_arithmetic<cjm::numerics::uint256> : std::true_type {};

	template<>
	struct is_integral<cjm::numerics::uint256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint256>::is_integer,
		std::true_type, std::false_type> {};


	template<>
	struct is_unsigned<cjm::numerics::uint256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint256>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::uint256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint256>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_arithmetic<cjm::numerics::int128> : std::true_type
	{

	};

	template<>
	struct is_integral<cjm::numerics::int128> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int128>::is_integer,
		std::true_type, std::false_type> {};


	template<>
	struct is_unsigned<cjm::numerics::int128> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int128>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::int128> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int128>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_arithmetic<cjm::numerics::int256> : std::true_type
	{

	};

	template<>
	struct is_integral<cjm::numerics::int256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int256>::is_integer,
		std::true_type, std::false_type> {};


	template<>
	struct is_unsigned<cjm::numerics::int256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int256>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::int256> :
		std::conditional_t<std::numeric_limits<cjm::numerics::int256>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_arithmetic<cjm::numerics::huge_decimal> : std::true_type
	{

	};

	template<>
	struct is_unsigned<cjm::numerics::huge_decimal> :
		std::conditional_t<std::numeric_limits<cjm::numerics::huge_decimal>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::huge_decimal> :
		std::conditional_t<std::numeric_limits<cjm::numerics::huge_decimal>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_floating_point<cjm::numerics::huge_decimal> : std::true_type {};

	template<>
	struct is_arithmetic<cjm::numerics::large_decimal> : std::true_type
	{

	};

	template<>
	struct is_unsigned<cjm::numerics::large_decimal> :
		std::conditional_t<std::numeric_limits<cjm::numerics::large_decimal>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::large_decimal> :
		std::conditional_t<std::numeric_limits<cjm::numerics::large_decimal>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_floating_point<cjm::numerics::large_decimal> : std::true_type {};

	template<>
	struct is_arithmetic<cjm::numerics::norm_decimal> : std::conditional_t<std::numeric_limits<cjm::numerics::norm_decimal>::is_specialized,
		std::true_type, std::false_type>
	{

	};

	template<>
	struct is_unsigned<cjm::numerics::norm_decimal> : std::conditional_t<std::numeric_limits<cjm::numerics::norm_decimal>::is_signed,
		std::false_type, std::true_type> {};

	template<>
	struct is_signed<cjm::numerics::norm_decimal> : std::conditional_t<std::numeric_limits<cjm::numerics::norm_decimal>::is_signed,
		std::true_type, std::false_type> {};

	template<>
	struct is_floating_point<cjm::numerics::norm_decimal> : std::conditional_t<std::numeric_limits<cjm::numerics::norm_decimal>::is_integer,
		std::false_type, std::true_type> {};

}

#include "numerics.inl"
#endif
