#ifndef CJM_CLANG_TEST_INTRINSICS_HPP_
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <climits>
#include <bit>
#include <span>
#define CJM_CLANG_TEST_INTRINSICS_HPP_
#if defined (_MSC_VER) && defined(_M_X64)
#define CJM_CLANG_TEST_INTRINSICS_AVAILABLE
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
#ifdef CJM_TEST_NUMERICS_UINT128_INTEL_BMI2
#define CJM_TEST_UMUL128 _mulx_u64
#else
#define CJM_TEST_UMUL128 _umul128
#endif
#define CJM_TEST_BITSCAN_REV_64 _BitScanReverse64
#define CJM_TEST_LZCNT_64 __lzcnt64
#define CJM_TEST_UDIV128 _udiv128
#define CJM_TEST_LSHIFT128 __shiftleft128
#define CJM_TEST_RSHIFT128 __shiftright128
#ifdef CJM_TEST_NUMERICS_UINT128_INTEL_ADX
#define CJM_TEST_ADDCARRY64 _addcarryx_u64
#else
#define CJM_TEST_ADDCARRY64 _addcarry_u64
#endif
#define CJM_TEST_SUBBORROW_64 _subborrow_u64
#else
#define CJM_TEST_UMUL128 cjm::clang::test_program::intrinsics_test::cjm_test_bad_umul128
#define CJM_TEST_BITSCAN_REV_64 cjm::clang::test_program::intrinsics_test::cjm_test_badrev_bitscan_64
#define CJM_TEST_LZCNT_64 cjm::clang::test_program::intrinsics_test::cjm_test_bad_lzcnt_64
#define CJM_TEST_UDIV128 cjm::clang::test_program::intrinsics_test::cjm_test_bad_udiv128
#define CJM_TEST_LSHIFT128 cjm::clang::test_program::intrinsics_test::cjm_test_bad_shiftleft128
#define CJM_TEST_RSHIFT128 cjm::clang::test_program::intrinsics_test::cjm_test_bad_shiftright128
#define CJM_TEST_ADDCARRY64 cjm::clang::test_program::intrinsics_test::cjm_test_bad_addc64
#define CJM_TEST_SUBBORROW_64 cjm::clang::test_program::intrinsics_test::cjm_test_bad_subb64
#endif

namespace  cjm::clang::test_program::intrinsics_test
{
	void print_intrinsic_availability_info();
}

#endif
