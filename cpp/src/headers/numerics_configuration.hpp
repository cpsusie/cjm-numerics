// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#ifndef CJM_NUMERICS_CONFIGURATION_HPP_
#define CJM_NUMERICS_CONFIGURATION_HPP_
#ifdef _MSC_VER
#define CJM_MSC
#endif
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#pragma intrinsic(_umul128)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(__lzcnt64)
#pragma intrinsic(_udiv128)  // NOLINT(clang-diagnostic-ignored-pragma-intrinsic)
#pragma intrinsic(__shiftleft128)
#pragma intrinsic(__shiftright128)
#ifndef CJM_MSC_X64
#define CJM_MSC_X64
#define CJM_UMUL128 _umul128
#define CJM_BITSCAN_REV_64 _BitScanReverse64
#define CJM_LZCNT_64 __lzcnt64
#define CJM_UDIV128 _udiv128
#define CJM_LSHIFT128 __shiftleft128
#define CJM_RSHIFT128 __shiftright128
#define CJM_ADDCARRY64 _addcarry_u64
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

namespace cjm
{

	namespace numerics
	{
		namespace internal
		{
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



		enum class uint128_calc_mode : std::uint8_t
		{
			default_eval = 0x00,
			msvc_x64,
			intrinsic_u128,
		};
		constexpr uint128_calc_mode init_eval_mode() noexcept;
	}
}


#endif
#include "numerics_configuration.inl"
