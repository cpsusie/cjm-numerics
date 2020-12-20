#ifndef CJM_NUMERICS_HPP
#define CJM_NUMERICS_HPP
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#pragma intrinsic(_umul128)
#ifndef CJM_MSC_X64
#define CJM_MSC_X64
#define CJM_UMUL128 _umul128
#endif
#else
#define CJM_UMUL128 cjm_bad_umul128
#endif
#include <cmath>
#include <limits>
#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <boost/functional/hash.hpp>
#include <numeric>
#include <cstring>
#include "cjm_numeric_concepts.hpp"
#ifdef __cpp_lib_bit_cast
#include <bit>
#endif

namespace cjm
{
	
	namespace numerics
	{
	    //alternate declarations for cjm_intrinsic_macros ... never defined because never used but need something that won't blow compiler up
		//when examining untaken if constexpr branch.
		extern std::uint64_t cjm_bad_umul128(std::uint64_t multiplicand, std::uint64_t multiplicand_two, std::uint64_t* carry);
		class uint128;
	    constexpr bool has_msc_x64 =
#ifdef CJM_MSC_X64
        true;
#else
	    false;
#endif

	    constexpr bool has_intrinsic_u128 =
#ifdef __SIZEOF_INT128__
        true;
		using uint128_align_t = unsigned __int128;
		using natuint128_t = unsigned __int128;
#ifndef CJM_HAVE_BUILTIN_128
#define CJM_HAVE_BUILTIN_128
#endif
#else
	    false;
		using uint128_align_t = std::uint64_t;
		using natuint128_t = uint128;
#ifdef CJM_HAVE_BUILTIN_128
#undef CJM_HAVE_BUILTIN_128
#endif
#endif
	    constexpr size_t uint128_align = alignof(uint128_align_t);


		

		enum class uint128_calc_mode : std::uint8_t
        {
		    default_eval = 0x00,
		    msvc_x64,
		    intrinsic_u128,
        };
		constexpr uint128_calc_mode init_eval_mode() noexcept;

#ifdef __cpp_lib_bit_cast
		template<typename To, typename From>
	       requires cjm::numerics::concepts::bit_castable<To, From>
	    constexpr To bit_cast(const From& f) noexcept
	    {
	        return std::bit_cast<To, From>(f);
	    }
#else
        template<typename To, typename From>
            requires cjm::numerics::concepts::bit_castable<To, From>
        To bit_cast(const From& f) noexcept
        {
            //GCC seems to get all but hurt about private member variables even if type is trivial.
            //All c++ standard requires is trivially copyable and same size.  the concepts here
            //enforce MORE than that requirement (also require triviality in general and alignment same)
            //Suppressing because uint128 is a type that is trivially copyable-to.
            To dst;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
            std::memcpy(&dst, &f, sizeof(To));  /* no diagnostic for this one */
#pragma GCC diagnostic pop
            return dst;
        }
#endif

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
		#pragma warning(push)
		#pragma warning (disable:4068) 
        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "UnreachableCode"
        constexpr uint128_calc_mode init_eval_mode() noexcept
        {
            if constexpr (has_intrinsic_u128)
            {
                return uint128_calc_mode::intrinsic_u128;
            }
            else if constexpr (has_msc_x64)
            {
                return uint128_calc_mode::msvc_x64;
            }
            else
            {
                return uint128_calc_mode::default_eval;
            }
        }
        #pragma clang diagnostic pop
		#pragma warning(pop)
	}
}



namespace std
{
	template<>
	struct hash<cjm::numerics::uint128>;

	template<>
	class numeric_limits<cjm::numerics::uint128>;

	/*template <>
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
		std::false_type, std::true_type> {};*/

}

#include "numerics.inl"
#endif
