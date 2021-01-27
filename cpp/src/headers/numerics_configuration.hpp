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
#endif
#else
#define CJM_UMUL128 internal::cjm_bad_umul128
#define CJM_BITSCAN_REV_64 internal::cjm_badrev_bitscan_64
#define CJM_LZCNT_64 internal::cjm_bad_lzcnt_64
#define CJM_UDIV128 internal::cjm_bad_udiv128
#define CJM_LSHIFT128 internal::cjm_bad_shiftleft128
#define CJM_RSHIFT128 internal::cjm_bad_shiftright128
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

namespace cjm::numerics::configuration
{
	
}

#endif
