#include <cjm/numerics/uint128.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
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
#include <optional>
#include <concepts>
#include <compare>

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

//NOTE: Intel Classic compiler is not currently supported.  Support will be added when/if Intel Classic
//adds sufficient support for C++20.

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
	using divmod_result_t = typename uint128_t ::divmod_result_t;
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
	void demonstrate_binary_bitwise_operations();
	void say_hello();
	void say_goodbye();
	void demonstrate_runtime_division_and_modulus();
	void demonstrate_nonthrowing_runtime_division_and_modulus();
	void demonstrate_constexpr_division_and_modulus();
	void print_optional_divmod_result(const std::optional<divmod_result_t>& print_me);
	std::string_view ordering_text(std::strong_ordering ordering);
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
		demonstrate_runtime_division_and_modulus();
		demonstrate_nonthrowing_runtime_division_and_modulus();
		demonstrate_constexpr_division_and_modulus();
		demonstrate_binary_bitwise_operations();
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

void cjm::uint128::example_code::demonstrate_binary_bitwise_operations()
{
	cout << newl << "This is the binary bitwise operations demonstration." << newl;
	
	constexpr auto left_operand =	256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	constexpr auto right_operand =	295'990'755'043'860'794'295'278'260'016'730'001'421_u128;

	auto print_result = [=](uint128_t val, bool runtime, std::string_view op_name) -> void
	{
		cout << "Performing bitwise " << op_name << " on left and right operands yields [0x" << std::hex << std::setw(std::numeric_limits<uint128_t>::digits / 4) << std::setfill('0') << val << "] at " << (runtime ? " run-time." : "compile-time.") << std::dec << newl;
	};
	
	auto runtime_or_res = left_operand | right_operand;
	auto runtime_and_res = left_operand & right_operand;
	auto runtime_xor_res = left_operand ^ right_operand;
	constexpr auto ctime_or_res = left_operand | right_operand;
	constexpr auto ctime_and_res = left_operand & right_operand;
	constexpr auto ctime_xor_res = left_operand ^ right_operand;

	print_result(runtime_or_res, true, "or"sv);
	print_result(runtime_and_res, true, "and"sv);
	print_result(runtime_xor_res, true, "xor"sv);

	print_result(ctime_or_res, false, "or"sv);
	print_result(ctime_and_res, false, "and"sv);
	print_result(ctime_xor_res, false, "xor"sv);
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
	cout << newl << newl << "The demonstration of CJM's uint128 type has completed.  We hope you found it useful." << std::endl;
}

void cjm::uint128::example_code::demonstrate_runtime_division_and_modulus()
{
	cout << newl << "This is the simple runtime division demonstration." << newl;
	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;
	//note that numeric limits is specialized appropriately.
	constexpr auto zero = std::numeric_limits<uint128_t>::min();
	uint128_t quotient = dividend / divisor;
	uint128_t remainder = dividend % divisor;

	//division and modulus can also be done together.
	auto [quotient_2, remainder_2] = uint128_t::div_mod(dividend, divisor);

	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;
	cout << "Obtained via divmod function: == [" << quotient_2 << "] with a remainder of [" << remainder_2 << "]." << newl;

	//for the characteristics of divmod_result_t, consult demonstrate_constexpr_division_and_modulus, or the header
	//at <cjm/numerics/cjm_numeric_concepts.hpp>

	//Division and modulus are somewhat unique because they will throw std::domain_error on a divisor of 0
	//rather than exhibiting undefined behavior.  Most other operations (such as shift by >= number of binary digits)
	//exhibit undefined behavior to avoid the expense of input validation.  Operations like shift etc however
	//are very fast operations and I did not want to introduce an additional branch.  Division, however,
	//is a slow operation already and one doubts that a precondition check is going to add much if anything to it.

	//Example -- just division:
	std::optional<uint128_t> res;
	std::optional<typename uint128_t::divmod_result_t> divmod_res;
	try
	{
		res = dividend / zero;
		auto strm = std::stringstream{};
		strm << "The foregoing was supposed to throw an exception but instead yielded a quotient of [" << res.value() << "]." << newl;
		throw std::logic_error{strm.str()};
	}
	catch (const std::domain_error& ex)
	{
		std::cout << "This is correct behavior. Division and modulus operators and the divmod function will throw on divisor == 0.  Message: [" << ex.what() << "]." << newl;
	}
	catch (const std::logic_error&)
	{
		throw;
	}
	catch (...)
	{
		throw std::logic_error{"The exception was supposed to be a domain error."};
	}

	//Example: divmod
	try
	{
		divmod_res = uint128_t::div_mod(dividend, zero);
		auto strm = std::stringstream{};
		strm << "The foregoing was supposed to throw an exception but instead yielded a quotient of [" << divmod_res.value().quotient << "] and a remainder of [" << divmod_res.value().remainder << "]." << newl;
		throw std::logic_error{strm.str()};
	}
	catch (const std::domain_error& ex)
	{
		std::cout << "This is correct behavior. Division and modulus operators and the divmod function will throw on divisor == 0.  Message: [" << ex.what() << "]." << newl;
	}
	catch (const std::logic_error&)
	{
		throw;
	}
	catch (...)
	{
		throw std::logic_error{"The exception was supposed to be a domain error."};
	}
}

void cjm::uint128::example_code::demonstrate_nonthrowing_runtime_division_and_modulus()
{

	cout << newl << "This is the nonthrowing runtime division demonstration." << newl;
	//Im sure some people will worry about the performance for checking zero so I have provided non-throwing
	//alternatives.

	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;
	//note that numeric limits is specialized appropriately.
	constexpr auto zero = std::numeric_limits<uint128_t>::min();

	//the following if supplied a zero divisor will exhibit undefined behavior:
	//no precondition check made
	auto [quotient, remainder] = uint128_t::unsafe_div_mod(dividend, divisor);
	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;

	//if you would like a way to have precondition-checked division but do not like exceptions for some fool reason,
	//the following works too.
	std::optional<divmod_result_t> safe_divmod_result = uint128_t::try_div_mod(dividend, divisor);
	cout << "Here is the result -- success case try_divmod:\t";
	print_optional_divmod_result(safe_divmod_result);
	cout << newl;

	//zero divisor will yield error signaled by nullopt return value:
	safe_divmod_result = uint128_t::try_div_mod(dividend, zero);
	cout << "Here is the result -- failure case try_divmod:\t";
	print_optional_divmod_result(safe_divmod_result);
	cout << newl;
}

void cjm::uint128::example_code::print_optional_divmod_result(const std::optional<divmod_result_t>& print_me)
{
	if (print_me.has_value())
	{
		cout << "Quotient: [" << print_me->quotient << "]; Remainder: [" << print_me->remainder << "]";
	}
	else
	{
		cout << "NullOpt/ErrorResult";
	}
}

void cjm::uint128::example_code::demonstrate_constexpr_division_and_modulus()
{
	cout << newl << "This is the compile-time division demonstration." << newl;
	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;

	//You can use division or modulus or the compound function uint128_t::div_mod in a constexpr context.
	constexpr auto quotient = dividend / divisor;
	constexpr auto remainder = dividend % divisor;
	constexpr divmod_result_t quo_rem_2 = uint128_t::div_mod(dividend, divisor);
	constexpr divmod_result_t quo_rem_3 = uint128_t::div_mod(4_u128, 2_u128);

	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;
	cout << "Obtained via divmod function: == [" << quo_rem_2.quotient << "] with a remainder of [" << quo_rem_2.remainder << "]." << newl;
	static_assert(quotient == quo_rem_2.quotient && remainder == quo_rem_2.remainder, "In a constexpr context calculation results can be used in static assertion.");
	static_assert(quo_rem_3.quotient == 2_u128 && quo_rem_3.remainder == 0_u128, "4/2 == 2, no remainder");

	//unlike with a non-constexpr context, attempting to divide by zero in a constexpr context will generate a compilation error
	//because of a lack of constant expression
	//if it were bad_quotient/bad_remainder were not constexpr, computation would be deferred to runtime and then throw an exception
	//(following two lines will not compile):
	//constexpr auto bad_quotient = dividend / 0_u128;
	//constexpr auto bad_remainder = dividend % 0_u128;

	//another constexpr division / modulus option is try_div_mod
	constexpr std::optional<divmod_result_t> divide_by_zero = uint128_t::try_div_mod(dividend, 0);
	static_assert(divide_by_zero == std::nullopt, "Divide by zero yields std::nullopt.");
	print_optional_divmod_result(divide_by_zero);
	cout << newl;

	//note that div_mod_t is constexpr hashable and constexpr totally ordered
	constexpr size_t hash_1 = std::hash<divmod_result_t>{}(quo_rem_2);
	constexpr size_t hash_2 = std::hash<divmod_result_t>{}(quo_rem_3);
	constexpr std::strong_ordering comp_res = quo_rem_2 <=> quo_rem_3;
	cout << "Constexpr hash of first divmod_result_t: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << hash_1 << "]." << newl;
	cout << "Constexpr hash of second divmod_result_t: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << hash_2 << "]." << newl;
	cout << "Their ordering: [" << ordering_text(comp_res) << "]." << newl;
	static_assert(concepts::nothrow_hashable<divmod_result_t> && std::totally_ordered<divmod_result_t>, "Meet nothrow hashable and totally ordered concepts.");
}

std::string_view cjm::uint128::example_code::ordering_text(std::strong_ordering ordering)
{
	using namespace std::string_view_literals;
	if (ordering == std::strong_ordering::equal || ordering == std::strong_ordering::equivalent)
	{
		return "EQUAL"sv;
	}
	else if (ordering == std::strong_ordering::greater)
	{
		return "GREATER"sv;
	}
	else
	{
		return "LESS"sv;
	}
}
