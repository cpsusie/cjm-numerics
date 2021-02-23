#include <cjm/numerics/uint128.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <limits>
#include <type_traits>
#include <climits>
#include <cstdint>
#include <utility>
#include <optional>
#include <exception>
#include <stdexcept>
//The purpose of this EXAMPLE_CODE is to demonstrate the functionality of the CJM uint128 type,
//show how it works, and talk somewhat about its strategies on various systems. It does NOT attempt
//to prove correctness.  Proof of correctness is available through the uint128_test_app
//which contains extensive unit tests.  THIS application, unlike uint128_test_app
//has NO dependencies other than a C++20 compiler that conforms to a subset of the C++20
//standard a C++20 standard library implementation that does the same.  Please
// consult this project's repository to learn what subset that is.  (Briefly, on Windows (tested
// 32 and 64 bit Microsoft 19.28+, on Linux - GCC 10.2.0+ and Linux Clang 11.0.0-2+ are known to work in
// x64 system.  x86 has not been tested.)  Hopefully, the dependency free nature of this header-only library
// and example code application will be convenient for learning about the library.

//By contrast, the UNIT TEST APPLICATION carries with it dependencies including:
	//boost (https://github.com/boostorg)
	//abseil[cxx17] (https://github.com/abseil/abseil-cpp)
	//date (https://github.com/HowardHinnant/date)
//All of the foregoing are available as vcpkg ports and were easily integrated for use in unit tests as follows:
	//x64-windows-static (visual studio, microsoft compiler, 64-bit Intel/Amd (x86-64) static linking of dependencies, including CRT)
	//x86-windows-static (visual studio, microsoft compiler, 32-bit Intel/Amd (x86), static linking of dependencies, including CRT)
	//x64-linux (g++ 10.2.0 and clang 11.0.0.2 using libstdc++ on Kubuntu 20.10, default linking, using CLion as development environment)

//I did not want to require you to have to build anything beyond what is required by the library for the example / demonstration code
//because the library itself makes no use of such.

namespace cjm::uint128::example_code
{
	using uint128_t = numerics::uint128;
	using namespace uint128_literals;
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using std::cout;
	constexpr auto newl = '\n';
	//constexpr auto wnewl = L'\n';

	void demonstrate_subtraction();
	void demonstrate_constexpr_subtraction();
	void demonstrate_addition();
	void demonstrate_constexpr_addition();
	void demonstrate_multiplication();
	void demonstrate_constexpr_multiplication();
	void say_hello();
	void say_goodbye();
	void demonstrate_runtime_division();
}

int main()
{
	using namespace cjm::uint128::example_code;
	try
	{
		say_hello();
		demonstrate_addition();
		demonstrate_constexpr_addition();
		demonstrate_subtraction();
		demonstrate_constexpr_subtraction();
		demonstrate_multiplication();
		demonstrate_constexpr_multiplication();
		demonstrate_runtime_division();
		say_goodbye();		
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::cerr << "example_code program for uint128 threw unexpected exception with message: [" << ex.what() << "]." << newl;
		}
		catch (...)
		{
			//eat it
		}
		return -1;
	}
	catch (...)
	{
		try
		{
			std::cerr << "example_code program for uint128 threw unexpected non-standard exception." << newl;
		}
		catch (...)
		{
			//eat it
		}
		return -1;
	}
	return 0;
}

void cjm::uint128::example_code::demonstrate_constexpr_subtraction()
{
	cout << newl << "This is the compile-time subtraction demonstration." << newl;
	constexpr auto minuend = std::numeric_limits<uint128_t>::max();
	constexpr auto subtrahend = 256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	constexpr auto difference = minuend - subtrahend;

	//subtraction with borrow takes a carry-in argument and returns a pair, the first of which is the difference and the second of which is borrow_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	constexpr auto sbb_res = sub_with_borrow(minuend, subtrahend, 0);
	cout
		<< "Subtracting [" << subtrahend << "] from [" << minuend << "] yields difference: [" << difference << "].  "
		<< "Subtracting with borrow (no borrow-in) yielded [" << sbb_res.first << "] as the difference and indicated that "
		<< (sbb_res.second == 0 ? "there was no borrow-out meaning the operation did not overflow/underflow."
			: "there was borrow due to overflow/underflow.") << newl;
}


void cjm::uint128::example_code::demonstrate_subtraction()
{
	cout << newl << "This is the runtime subtraction demonstration." << newl;
	const auto minuend = std::numeric_limits<uint128_t>::max();
	const auto subtrahend = 256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	const auto difference = minuend - subtrahend;

	//subtraction with borrow takes a carry-in argument and returns a pair, the first of which is the difference and the second of which is borrow_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	auto [diff_result, borrow_out] = sub_with_borrow(minuend, subtrahend, 0);
	cout
		<< "Subtracting [" << subtrahend << "] from [" << minuend << "] yields difference: [" << difference << "].  "
		<< "Subtracting with borrow (no borrow-in) yielded [" << diff_result << "] as the difference and indicated that "
		<< (borrow_out == 0 ? "there was no borrow-out meaning the operation did not overflow/underflow."
			: "there was borrow due to overflow/underflow.") << newl;
}

void cjm::uint128::example_code::demonstrate_addition()
{
	cout << newl << "This is the runtime addition demonstration." << newl;
	//123 septillion, 792 sextillion ... etc
	const auto first_addend = 123'792'321'010'356'001'641'481'971_u128;
	//888 octillion, 911 septillion ... etc
	const auto second_addend = 888'911'410'443'901'902'103'000'301'000_u128;
	//addition
	const auto sum = first_addend + second_addend;
	//addition with carry takes a carry-in argument and returns a pair, the first of which is the sum and the second of which is carry_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	auto [sum_result, carry_out] =   add_with_carry(first_addend, second_addend, 0);
	cout
		<< "Adding [" << first_addend << "] to [" << second_addend << "] yields sum: [" << sum << "].  "
		<< "Adding with carry yielded [" << sum_result << "] as the sum and indicated that "
		<< (carry_out == 0 ? "there was no carry-out meaning the operation did not overflow." 
			: "there was carry-out due to overflow.") << newl;
}

void cjm::uint128::example_code::demonstrate_constexpr_addition()
{
	cout << newl << "This is the compile-time addition demonstration." << newl;
	//123 septillion, 792 sextillion ... etc
	constexpr auto first_addend =	123'792'321'010'356'001'641'481'971_u128;
	//888 octillion, 911 septillion ... etc
	constexpr auto second_addend = 888'911'410'443'901'902'103'000'301'000_u128;
	//addition
	constexpr auto sum = first_addend + second_addend;
	//addition with carry takes a carry-in argument and returns a pair, the first of which is the sum and the second of which is carry_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were not constexpr, you could, of course, use structured bindings to make it more convenient.
	constexpr auto add_with_carry_res = add_with_carry(first_addend, second_addend, 0);
	static_assert(add_with_carry_res.first == sum && add_with_carry_res.second == 0, "They really ought to match and not overflow!");
	cout
		<< "Adding [" << first_addend << "] to [" << second_addend << "] yields sum: [" << sum << "].  "
		<< "There was" << (add_with_carry_res.second == 0 ? " no overflow." : " overflow.") << newl;
}

void cjm::uint128::example_code::demonstrate_multiplication()
{
	cout << newl << "This is the runtime multiplication demonstration." << newl;
	const auto first_factor = 1'693'490'369'045_u128;
	const auto second_factor = 2'309'948'587'417'034'374'169_u128;
	auto product = first_factor * second_factor;
	cout << "Multiplying [" << first_factor << "] by [" << second_factor << "] yields [" << product << "]." << newl;	
}

void cjm::uint128::example_code::demonstrate_constexpr_multiplication()
{
	cout << newl << "This is the compile-time multiplication demonstration." << newl;
	constexpr auto first_factor = 1'693'490'369'045_u128;
	constexpr auto second_factor = 2'309'948'587'417'034'374'169_u128;
	constexpr auto product = first_factor * second_factor;
	cout << "Multiplying [" << first_factor << "] by [" << second_factor << "] yields [" << product << "]." << newl;
}

void cjm::uint128::example_code::say_hello()
{
	cout << "Hello, welcome to the demonstration of CJM's uint128 type!" << newl << newl;
	cout << "Detected compiler: [" << get_text_narrow(compiler) << "]." << newl;
	cout << "Calculation mode: [";
	if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
	{
		cout << "IntrinsicU128]." << newl;
	}
	else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
	{
		cout << "msvc_x64_clang_or_llvm_intel]." << newl;
	}
	else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
	{
		cout << "msvc_x64]." << newl;
	}
	else
	{
		cout << "fallback]." << newl;
	}
	cout << "Intel Adx Available: [" << std::boolalpha << intel_adx_available << "]." << newl;
	cout << "Intel BMI2 Available: [" << std::boolalpha << intel_bmi2_available << "]." << newl;
	cout << "Sse3 available: [" << std::boolalpha << sse3_available << "]." << newl;
}

void cjm::uint128::example_code::say_goodbye()
{
	cout << newl << "The demonstration of CJM's uint128 type has completed.  We hope you found it useful." << std::endl;
}

void cjm::uint128::example_code::demonstrate_runtime_division()
{
	cout << newl << "This is the simple runtime division demonstration." << newl;
	uint128_t quotient = 0xc0de'd00d'fea2'b00b'cafe'babe'600d'f00d_u128 / 0xbadd'f00d_u128;
	cout << "Division result: [" << quotient << "]." << newl;
}
