#ifndef CJM_UINT128_TEST_PROGRAM_HPP_
#define CJM_UINT128_TEST_PROGRAM_HPP_
#include <concepts>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <limits>
#include <cstdint>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/uint128.hpp>
#include "int128_tests.hpp"
#include <boost/io/ios_state.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <exception>
#include <stdexcept>
#include <cjm/string/cjm_string.hpp>
#include <fstream>
#include <locale>
#include <tuple>
#include "int128_test_switches.hpp"
#include <cjm/numerics/fixed_uint_container.hpp>

namespace cjm::base_test_program
{
	using cout_saver = cjm::uint128_tests::cout_saver;
	constexpr auto newl = '\n';
	template<typename T>
	concept unsigned_integer = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

	template<unsigned_integer TUInt>
	void print_spec(TUInt val)
	{
		cout_saver{ std::cout };
		std::cout << std::hex << "[0x" << val << "]." << newl;
	}

	template<typename T>
	void print_spec(T val)
	{
		auto saver = cout_saver{ std::cout };
		std::cout << "Thing that isn't unsigned integer: [" << std::dec << val << "]." << newl;
	}

	int execute_test_program(int argc, char* argv[]) noexcept;

	void native_test();

	void log_arg_processing(int argc, char* argv[]);
	
	int print_args(int argc, char* argv[]) noexcept;
	
	int execute_log_arg_processing(int argc, char* argv[]) noexcept;
}
#endif //CJM_UINT128_TEST_PROGRAM_HPP_
