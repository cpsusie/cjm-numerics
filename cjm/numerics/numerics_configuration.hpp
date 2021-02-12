#ifndef CJM_NUMERICS_CONFIGURATION_HPP_
#define CJM_NUMERICS_CONFIGURATION_HPP_
#include <climits>
#include <cmath>
#include <limits>
#include <type_traits>
#include <numeric>
#include <cstring>
#include <functional>
#include "cjm_numeric_concepts.hpp"
#include <bit>
#include <array>
#include <string_view>
#include <span>
#include <cstdint>
#ifndef __cpp_char8_t
#error "CJM NUMERICS UINT128 requires a C++20 implementation that supports char8_t."
#endif
#ifndef  __cpp_concepts
#error "CJM NUMERICS UINT128 requires support for C++20 concepts."
#endif
#ifndef  __cpp_impl_three_way_comparison
#error "CJM NUMERICS UINT128 requires support for C++20 three way comparison operator <=>."
#endif
#ifndef __cpp_nontype_template_args
#error	"CJM NUMERICS UINT128 plans to require support for C++20 class non-type template arguments."
#endif
#ifndef __cpp_lib_bitops
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 bit operations."
#endif
#ifndef __cpp_lib_char8_t
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 char8_t type."
#endif
#ifndef __cpp_lib_concepts
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 concepts."
#endif
#ifndef __cpp_lib_endian
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 std::endian."
#endif
#ifndef __cpp_lib_is_constant_evaluated
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 std::is_constant_evaluated."
#endif
#ifndef __cpp_lib_is_nothrow_convertible
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 std::is_nothrow_convertible."
#endif
#ifndef __cpp_lib_remove_cvref
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 std::remove_cvref."
#endif
#ifndef __cpp_lib_span
#error	"CJM NUMERICS UINT128 requires standard library support for C++20 std::span."
#endif
#ifndef __cpp_lib_starts_ends_with
#error	"CJM NUMERICS UINT128 requires standard library support for starts_with and ends_with in std::string and std::string_view."
#endif
#ifdef _MSC_VER
#define CJM_MSC
#endif
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#include <immintrin.h>
#pragma intrinsic(_umul128)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(__lzcnt64)
#pragma intrinsic(_udiv128)  // NOLINT(clang-diagnostic-ignored-pragma-intrinsic)
#pragma intrinsic(__shiftleft128)
#pragma intrinsic(__shiftright128)
#pragma intrinsic(_addcarry_u64)
#pragma intrinsic(_addcarryx_u64)
#pragma intrinsic(_subborrow_u64)
#pragma intrinsic(_mulx_u64)
#ifndef CJM_MSC_X64
#define CJM_MSC_X64
#ifdef CJM_NUMERICS_UINT128_INTEL_BMI2
#define CJM_UMUL128 _mulx_u64
#else
#define CJM_UMUL128 _umul128
#endif
#define CJM_BITSCAN_REV_64 _BitScanReverse64
#define CJM_LZCNT_64 __lzcnt64
#define CJM_UDIV128 _udiv128
#define CJM_LSHIFT128 __shiftleft128
#define CJM_RSHIFT128 __shiftright128
#ifdef CJM_NUMERICS_UINT128_INTEL_ADX
#define CJM_ADDCARRY64 _addcarryx_u64
#else
#define CJM_ADDCARRY64 _addcarry_u64
#endif
#define CJM_SUBBORROW_64 _subborrow_u64
#endif
#else
#define CJM_UMUL128 internal::cjm_bad_umul128
#define CJM_BITSCAN_REV_64 internal::cjm_badrev_bitscan_64
#define CJM_LZCNT_64 internal::cjm_bad_lzcnt_64
#define CJM_UDIV128 internal::cjm_bad_udiv128
#define CJM_LSHIFT128 internal::cjm_bad_shiftleft128
#define CJM_RSHIFT128 internal::cjm_bad_shiftright128
#define CJM_ADDCARRY64 internal::cjm_bad_addc64
#define CJM_SUBBORROW_64 internal::cjm_bad_subb64
#endif


#ifdef __cpp_lib_bit_cast
#define CJM_BIT_CAST_CONST constexpr
#else
#define CJM_BIT_CAST_CONST inline
#endif
#ifdef __cpp_lib_bitops
#define CJM_HAS_BITOPS
#endif
#if defined (__GNUG__) || defined(__GNUC__)
#define CJM_IS_GCC 1
#endif
#if defined (CJM_IS_GCC) && !defined(__clang__)
#define CJM_IS_GCC_NOT_CLANG 1
#endif
#if defined (__clang__)
#define CJM_IS_CLANG
#endif
#if defined (CJM_IS_CLANG) && !defined(CJM_IS_GCC)
#define CJM_IS_CLANG_NOT_GCC
#endif
#if defined (CJM_NUMERICS_LITTLE_ENDIAN)
#error "CJM_NUMERICS_LITTLE_ENDIAN should not be set directly"
#endif
#if defined(CJM_MSC)
#define CJM_NUMERICS_LITTLE_ENDIAN true
#elif !defined(CJM_NUMERICS_LITTLE_ENDIAN) && (!defined(__BYTE_ORDER__) || ( !defined(__ORDER_LITTLE_ENDIAN__) && !defined(__ORDER_BIG_ENDIAN__)))
#error "Unable to detect endianness of system."
#elif !defined(CJM_NUMERICS_LITTLE_ENDIAN) && (__BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__)
#define CJM_NUMERICS_LITTLE_ENDIAN true
#elif (__BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__)
#define CJM_NUMERICS_LITTLE_ENDIAN false
#else
#error "Unable to detect endianness of system."
#endif



namespace cjm
{

	namespace numerics
	{
		namespace internal
		{
			//REQUIRE USUAL SIZES AND DIGIT COUNTS
			//REQUIRE DEFINITION of all optional fixed-width integer types in cstdint from 8-64 sizes
			//REQUIRE CHAR_BIT to be normal (number of bits in char/unsigned char/signed char to be 8)
			static_assert(CHAR_BIT == 8, "CJM NUMERICS is not designed for systems where CHAR_BIT != 8.");

			//require definition of std::uint8_t and std::int8_t, usual size and digit count
			static_assert(sizeof(std::uint8_t) == 1 && std::numeric_limits<std::uint8_t>::digits == 8,
			              "CJM NUMERICS requires definition of std::uint8_t to be a type 1 byte long and with 8 binary digits.");
			static_assert(sizeof(std::int8_t) == 1 && std::numeric_limits<std::int8_t>::digits == std::numeric_limits<std::uint8_t>::digits -1,
			              "CJM NUMERICS requires definition of std::int8_t to be a type 1 byte long and with 7 binary digits.");
			//require definition of std::uint16_t and std::int16_t, usual size and digit count
			static_assert(sizeof(std::uint16_t) == 2 && std::numeric_limits<std::uint16_t>::digits == 16,
					"CJM NUMERICS requires definition of std::uint16_t to be a type 2 bytes long and with 16 binary digits.");
			static_assert(sizeof(std::int16_t) == 2 && std::numeric_limits<std::int16_t>::digits == std::numeric_limits<std::uint16_t>::digits -1,
			              "CJM NUMERICS requires definition of std::int16_t to be a type 2 bytes long and with 15 binary digits.");
			//require definition of std::uint32_t and std::int32_t, usual size and digit count
			static_assert(sizeof(std::uint32_t) == 4 && std::numeric_limits<std::uint32_t>::digits == 32,
			              "CJM NUMERICS requires definition of std::uint32_t to be a type 2 bytes long and with 32 binary digits.");
			static_assert(sizeof(std::int32_t) == 4 && std::numeric_limits<std::int32_t>::digits == std::numeric_limits<std::uint32_t>::digits -1,
			              "CJM NUMERICS requires definition of std::int32_t to be a type 2 bytes long and with 31 binary digits.");
			//require definition of std::uint64_t and std::int64_t, usual size and digit count
			static_assert(sizeof(std::uint64_t) == 8 && std::numeric_limits<std::uint64_t>::digits == 64,
			              "CJM NUMERICS requires definition of std::uint64_t to be a type 8 bytes long and with 64 binary digits.");
			static_assert(sizeof(std::int64_t) == 8 && std::numeric_limits<std::int64_t>::digits == std::numeric_limits<std::uint64_t>::digits -1,
			              "CJM NUMERICS requires definition of std::int64_t to be a type 8 bytes long and with 63 binary digits.");
			static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4, "Only 32 and 64 bit architecture supported.");

			constexpr bool validate_uint128_concept_compliance_dev =
#ifndef CJM_NUMERICS_UINT128_VALIDATE_UINT128_CONCEPT_COMPLIANCE_DEV
				false;
#else
				true;
#endif
			
			//alternate declarations for cjm_intrinsic_macros ... never defined because never used but need something that won't blow compiler up
			//when examining untaken if constexpr branch.
			extern unsigned char cjm_badrev_bitscan_64(unsigned long* index, std::uint64_t mask);
			extern std::uint64_t cjm_bad_lzcnt_64(std::uint64_t mask);
			extern std::uint64_t cjm_bad_umul128(std::uint64_t multiplicand, std::uint64_t multiplicand_two, std::uint64_t* carry);
			extern std::uint64_t cjm_bad_udiv128(std::uint64_t high_dividend, std::uint64_t low_dividend, std::uint64_t divisor, std::uint64_t* remainder);
			extern std::uint64_t cjm_bad_shiftleft128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
			extern std::uint64_t cjm_bad_shiftright128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
			extern unsigned char cjm_bad_addc64(unsigned char c_in, std::uint64_t x, std::uint64_t y, std::uint64_t* sum) noexcept;
			extern unsigned char cjm_bad_subb64(unsigned char b_in, std::uint64_t minuend, std::uint64_t subtrahend, std::uint64_t* difference) noexcept;
		}

		enum class compiler_used : unsigned
		{
			other = 0,
			msvc,
			clang_gcc,
			gcc,
			clang,			
		};
		constexpr compiler_used min_value = compiler_used::other;
		constexpr compiler_used max_value = compiler_used::clang;
		constexpr std::string_view get_text_narrow(compiler_used v) noexcept;
		constexpr std::wstring_view get_text_wide(compiler_used v) noexcept;
		constexpr bool is_value_defined(compiler_used v) noexcept;

		constexpr compiler_used value_or_other_ifndef(compiler_used v) noexcept;
		
		class uint128;

		constexpr bool is_windows_x64 =
#ifdef CJM_MSC_X64
			true;
#else
			false;
#endif
		
		constexpr compiler_used compiler =
#ifdef CJM_MSC
			compiler_used::msvc;
#elif defined (CJM_IS_GCC_NOT_CLANG)
			compiler_used::gcc;
#elif defined (CJM_IS_CLANG_NOT_GCC)
			compiler_used::clang;
#elif defined (CJM_IS_GCC)
			compiler_used::clang_gcc;
#else
			compiler_used::other;
#endif		
		constexpr bool has_cpp20_bitops =
#ifdef CJM_HAS_BITOPS
			true;
#else
			false
#endif
			constexpr bool constexpr_bit_casting =
#ifdef __cpp_lib_bit_cast
			true;
#else
			false;
#endif
		constexpr bool has_msc_x64 =
#ifdef CJM_MSC_X64
			true;
#else
			false;
#endif
		constexpr bool has_msc =
#ifdef CJM_MSC
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
		namespace internal
		{
			struct alignas(alignof(std::uint64_t) * 2) cjm_align {
				std::uint64_t m_low;
				std::uint64_t m_high;
			};
		}
		using uint128_align_t = std::conditional_t<has_msc_x64, internal::cjm_align, std::uint64_t>;
		using natuint128_t = uint128;
#ifdef CJM_HAVE_BUILTIN_128
#undef CJM_HAVE_BUILTIN_128
#endif
#endif
		constexpr size_t uint128_align = alignof(uint128_align_t);
		constexpr bool sse3_available =
#if defined(__SSE3__)
			true;
#else
			false;
#endif
		constexpr bool intel_adx_available =
#if defined(CJM_NUMERICS_UINT128_INTEL_ADX)
			is_windows_x64;
#else
			false;
#endif
		constexpr bool intel_bmi2_available =
#if defined(CJM_NUMERICS_UINT128_INTEL_BMI2)
			is_windows_x64;
#else
			false;
#endif

		enum class uint128_calc_mode : std::uint8_t
		{
			default_eval = 0x00,
			msvc_x64,
			intrinsic_u128,
		};
		constexpr uint128_calc_mode init_eval_mode() noexcept;
	}

	namespace numerics::internal
	{
		static_assert(!validate_uint128_concept_compliance_dev || (numerics::concepts::builtin_floating_point<float> && numerics::concepts::builtin_floating_point<double> && numerics::concepts::builtin_floating_point<long double>));
		static_assert(!validate_uint128_concept_compliance_dev || numerics::concepts::builtin_unsigned_integer<std::uint64_t>);
		static_assert(!validate_uint128_concept_compliance_dev || numerics::concepts::builtin_unsigned_integer<std::uint8_t>);
		static_assert(!validate_uint128_concept_compliance_dev || concepts::division_modulus_result<divmod_result<std::uint64_t>, std::uint64_t>);
		static_assert(!validate_uint128_concept_compliance_dev || concepts::integer<std::uint64_t>);
		static_assert(!validate_uint128_concept_compliance_dev || std::is_standard_layout_v<divmod_result<std::uint64_t>>);
		static_assert(!validate_uint128_concept_compliance_dev || std::totally_ordered < divmod_result<std::uint64_t>>);
		
	}
	
}

#endif
#include <cjm/numerics/numerics_configuration.inl>