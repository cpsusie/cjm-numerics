#ifndef CJM_CLANG_TEST_PROGRAM_HPP_
#define CJM_CLANG_TEST_PROGRAM_HPP_
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
#include "test_intrinsics.hpp"

namespace cjm::clang::test_program
{
	constexpr auto newl = '\n';
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	
	template<typename Subroutine>
	int execute_subroutine_log_exception(Subroutine r, std::string_view routine_name, int failure_value_nz = -1) noexcept
	{
		try
		{
			r();
		}
		catch (const std::exception& ex)
		{
			try
			{
				auto stream = std::stringstream{};
				stream << "Exception throw during execution of routine [" << routine_name << "].  Message: [" << ex.what() << "]." << newl;
				std::cerr << stream.str() << newl;
			}
			catch (...)
			{
				//eat it
			}
			return failure_value_nz != 0 ? failure_value_nz : -1;
		}
		catch (...)
		{
			try
			{
				std::cerr << "Non-standard exception thrown during execution of routine: [" << routine_name << "]." << newl;
			}
			catch(...)
			{
				//eat it
			}
			return failure_value_nz != 0 ? failure_value_nz : -1;
		}
		return 0;
	}

	int execute_test_program() noexcept;
	void test_program();
	namespace environment 
	{
		constexpr bool is_microsoft =
#if defined(_MSC_VER)
			true;
#else
			false;
#endif
		constexpr bool is_x64 = sizeof(std::uintptr_t) == 8;
		constexpr bool is_windows_x64 =
#if defined(_M_X64)
			is_x64;
#else
			false;
#endif
		constexpr bool is_clang =
#ifdef __clang__
			true;
#else
			false;
#endif
		constexpr bool is_intel_classic =
#if defined(__ICL) || defined(_ICC)
			true;
#else
			false;
#endif
		constexpr bool is_intel_llvm_compiler =
#ifdef __INTEL_LLVM_COMPILER
			true;
#else
			false;
#endif
		constexpr bool has_concepts =
#ifdef  __cpp_concepts
			true;
#else
			false;
#endif
		constexpr bool has_char8_t =
#ifdef __cpp_char8_t
			true;
#else
			false;
#endif
		constexpr bool has_lib_bitops =
#ifdef __cpp_lib_bitops
			true;
#else
			false;
#endif
		constexpr bool has_lib_char8_t =
#ifdef __cpp_lib_char8_t
			true;
#else
			false;
#endif
		constexpr bool has_lib_concepts =
#ifdef __cpp_lib_concepts
			true;
#else
			false;
#endif
		constexpr bool has_lib_endian =
#ifdef __cpp_lib_endian
			true;
#else
			false;
#endif
		constexpr bool has_lib_constant_evaluated =
#ifdef  __cpp_lib_is_constant_evaluated
			true;
#else
			false;
#endif
		constexpr bool has_lib_nothrow_convertible =
#ifdef __cpp_lib_is_nothrow_convertible
			true;
#else
			false;
#endif
		constexpr bool has_lib_remove_cvref =
#ifdef __cpp_lib_remove_cvref
			true;
#else
			false;
#endif
		constexpr bool has_lib_span =
#ifdef __cpp_lib_span
			true;
#else
			false;
#endif
		constexpr bool has_lib_starts_and_ends_with =
#ifdef __cpp_lib_starts_ends_with
			true;
#else
			false;
#endif
		constexpr bool has_three_way_comparison =
#ifdef __cpp_impl_three_way_comparison
			true;
#else
			false;
#endif
		constexpr bool has_lib_bit_cast =
#ifdef __cpp_lib_bit_cast
			true;
#else
			false;
#endif
		constexpr bool has_intrinsic_u128 =
#ifdef __SIZEOF_INT128__
			true;
		using uint128_t = unsigned __int128;
#else
			false;
		using uint128_t = std::uint64_t;
#endif

		constexpr bool is_intrin_u128_arithmetic = has_intrinsic_u128 &&
			std::is_arithmetic_v<uint128_t>;
		constexpr bool is_gcc =
#if	defined(__GNUG__) || defined(__GNUC__)
			true;
#else
			false;
#endif
		constexpr bool has_consteval =
#ifdef __cpp_consteval
			true;
#else
			false;
#endif
	}

	namespace intrinsics_test
	{
		extern unsigned char cjm_test_badrev_bitscan_64(unsigned long* index, std::uint64_t mask);
		extern std::uint64_t cjm_test_bad_lzcnt_64(std::uint64_t mask);
		extern std::uint64_t cjm_test_bad_umul128(std::uint64_t multiplicand, std::uint64_t multiplicand_two, std::uint64_t* carry);
		extern std::uint64_t cjm_test_bad_udiv128(std::uint64_t high_dividend, std::uint64_t low_dividend, std::uint64_t divisor, std::uint64_t* remainder);
		extern std::uint64_t cjm_test_bad_shiftleft128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
		extern std::uint64_t cjm_test_bad_shiftright128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
		extern unsigned char cjm_test_bad_addc64(unsigned char c_in, std::uint64_t x, std::uint64_t y, std::uint64_t* sum) noexcept;
		extern unsigned char cjm_test_bad_subb64(unsigned char b_in, std::uint64_t minuend, std::uint64_t subtrahend, std::uint64_t* difference) noexcept;
	}
	
}
#endif

