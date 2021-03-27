// Copyright © 2020-2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
#include "uint128_test_program.hpp"

int cjm::base_test_program::print_args(int argc, char* argv[]) noexcept
{
	using namespace cjm::base_test_program;
	std::string exec;
	std::vector<std::string> vec;
	try
	{
		auto [exec_temp, arguments_temp] =
			cjm::uint128_tests::switches
				::normalize_and_stringify_console_args(argc, argv);
		exec = std::move(exec_temp);
		vec = std::move(arguments_temp);
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::cerr 
				<< "Unable to parse arguments.  Exception message: [" 
				<< ex.what() << "]." << newl;
		}
		catch (...)
		{
		}
		return -1;
	}
	catch (...)
	{
		try
		{
			std::cerr 
				<< "Unable to parse arguments.  Non-standard exception thrown." << newl;
		}
		catch (...)
		{

		}
		return -1;
	}

	try
	{
		std::cout << "Will list command line arguments." << newl;
		std::cout << "Executed: [" << exec << "]." << newl;
		size_t this_param = 0;
		const size_t num_params = vec.size();
		std::cout << "Will now print all [" << num_params << "] parameters: " << newl;
		for (const auto& str : vec)
		{
			std::cout 
				<< '\t' << "Parameter# " << (++this_param) << " of " 
				<< num_params << ": [" << str << "]." << newl;
		}
		std::cout << "Done printing command parameters." << newl;
		std::cout << "Done listing command line arguments." << newl << newl;
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::cerr 
				<< "Unable to parse arguments.  Exception message: [" 
				<< ex.what() << "]." << newl;
		}
		catch (...)
		{

		}
		return -1;
	}
	catch (...)
	{
		try
		{
			std::cerr << "Unable to parse arguments.  Non-standard exception thrown." << newl;
		}
		catch (...)
		{

		}
		return -1;
	}
	return 0;
}

int cjm::base_test_program::execute_log_arg_processing(int argc, char* argv[]) noexcept
{
	try
	{
		cjm::base_test_program::log_arg_processing(argc, argv);
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}

int cjm::base_test_program::execute_test_program(int argc, char* argv[]) noexcept
{
	try
	{
		using namespace numerics::uint128_literals;
		using uint128_t = numerics::uint128;
		using testing::cjm_assert;
		using namespace std::string_view_literals;

		static_assert(cjm::numerics::concepts::unsigned_integer<uint128_t>);
		std::vector<uint128_tests::test_switch> switch_vector;
		try
		{
			auto [exec_name, string_vec] =
				uint128_tests::switches::normalize_and_stringify_console_args(argc, argv);
			switch_vector = uint128_tests::switches::process_input(string_vec);
		}
		catch (const testing::testing_failure& ex)
		{
			std::cerr << "One or more unit tests FAILED but the program did everything properly up to that point." << newl;
			std::cerr << "Failure message: [" << ex.what() << "]" << newl;
			return 0;
		}
		catch (const std::runtime_error& ex)
		{
			std::cerr << "There was a problem, likely when processing command-line arguments." << newl;
			std::cerr << "Message: \"" << ex.what() << "\"." << newl;
			return -1;
		}
		catch (const std::exception& ex)
		{
			std::cerr << "An unexpected exception was thrown while trying"
				<< " to insert and extract an int with u16 string stream: ["
				<< ex.what() << "]." << newl;
			return -1;
		}

		try
		{
			uint128_tests::run_test_application(switch_vector);
		}
		catch (const testing::testing_failure& ex)
		{
			std::cerr << "One or more unit tests FAILED but the program did everything properly up to that point." << newl;
			std::cerr << "Failure message: [" << ex.what() << "]" << newl;
			return 0;
		}
		catch (const std::exception& ex)
		{
			std::cerr 
				<< "There was a problem (unrelated to a failed test) while executing the test program.  Message: [" 
				<< ex.what() << "]" << newl;
			return -1;
		}

	}
	catch (...)
	{
		try
		{
			std::cerr 
				<< "Test program FAULT!  Either a non-standard, unhandled exception was thrown.  Or an exception was thrown in a catch block." 
				<< "  The program must terminate.";
			return -1;
		}
		catch (...)
		{
			//naught else can do here....
			return -1;
		}
	}
	return 0;
}

void cjm::base_test_program::log_arg_processing(int argc, char* argv[])
{

	std::vector<std::string> vec;
	try
	{
		auto [exec_temp, arguments_temp] =
			cjm::uint128_tests::switches::normalize_and_stringify_console_args(argc, argv);
		vec = std::move(arguments_temp);
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::cerr << "Unable to parse arguments.  Exception message: [" << ex.what() << "]." << newl;
		}
		catch (...)
		{

		}
		throw;
	}
	catch (...)
	{
		try
		{
			std::cerr << "Unable to parse arguments.  Non-standard exception thrown." << newl;
		}
		catch (...)
		{

		}
		throw;
	}

	std::cout << "Successfully string-vectorized command line arguments." << newl;
	try
	{
		auto prepared = cjm::uint128_tests::switches::process_input(vec);
		const size_t num_switches = prepared.size();
		std::cout << "Successfully identified " << num_switches << " switches from command line." << newl;
		std::cout << "Will enumerate switches: " << newl;
		size_t this_switch = 0;

		for (const auto& arg_switch : prepared)
		{
			std::cout << "\tArg switch# " << (++this_switch) << " of " << num_switches << ":" << newl;
			std::cout << "\t\tSwitch value: [" << arg_switch.mode() << "]." << newl;
			if (arg_switch.file_path() != std::nullopt)
			{
				const auto& file = arg_switch.file_path().value();
				std::cout << "\t\tSwitch parameter: [" << file << "]" << newl;
			}
			else
			{
				std::cout << "\t\tSwitch lacks parameter." << newl;
			}
			std::cout << "\tDone Arg switch# " << this_switch << "." << newl;
		}
		std::cout << "Done enumerating switches." << newl << newl;
	}
	catch (const std::runtime_error& ex)
	{
		std::cerr << "There was a problem with one or more of the command line switches: " << newl;
		std::cerr << "[" << ex.what() << "]" << newl;
		std::cerr << "Command line arguments were NOT valid." << newl << newl;
	}
	catch (const std::logic_error& ex)
	{
		std::cerr << "Logic error parsing command line arguments.  This requires debugging. Msg: [" << ex.what() << "]." << newl;
		throw;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Unexpected error parsing command line arguments.  This requires debugging. Msg: [" << ex.what() << "]." << newl;
		throw;
	}
	catch (...)
	{
		std::cerr << "Non-standard error encountered parsing command line arguments.  This requires debugging." << newl;
		throw;
	}

}

#ifdef CJM_HAVE_BUILTIN_128
void cjm::base_test_program::native_test()
{
	using namespace cjm::numerics::uint128_literals;
	using uint128_t = cjm::numerics::uint128;
	constexpr std::uint64_t low = 0xdead'beef'face'babe;
	constexpr std::uint64_t high = 0xc0de'd00d'fea2'b00b;
	using u128native_t = unsigned __int128;
	constexpr u128native_t is_constexpr = 0xdead'beef'cafe'babe;
	u128native_t native = high;
	native <<= 64;
	native |= low;
	u128native_t copy_native = native;
	uint128_t from_native = static_cast<uint128_t>(native);
	static_assert(is_constexpr == 0xdead'beef'cafe'babe);
	cjm::testing::cjm_deny(is_constexpr == 0);
	std::uint64_t rt_low = static_cast<std::uint64_t>(copy_native);
	copy_native >>= 64;
	std::uint64_t rt_high = static_cast<std::uint64_t>(copy_native);

	cjm::testing::cjm_assert(low == rt_low && high == rt_high);
	constexpr auto my_literal = 0xc0de'd00d'fea2'b00b'dead'beef'face'babe_u128;
	cout_saver saver{ std::cout };
	cjm::testing::cjm_assert(from_native == my_literal);
	std::cout << "from native: [0x" << std::hex << from_native << "]." << newl;
}
#else
void cjm::base_test_program::native_test()
{
	using namespace cjm::base_test_program;
	std::cout << "Native uint128 not supported in this environment.  Will not run native tests." << newl;
}
#endif