#include <concepts>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <limits>
#include <cstdint>
#include "numerics.hpp"
#include "uint128.hpp"
#include "int128_tests.hpp"
#include <boost/io/ios_state.hpp>
#include "cjm_numeric_concepts.hpp"
#include <exception>
#include <stdexcept>
#include "cjm_string.hpp"
#include <fstream>
#include <locale>
#include <tuple>
#include "int128_test_switches.hpp"

namespace cjm::base_test_program
{
    using cout_saver = cjm::uint128_tests::cout_saver;
    constexpr auto newl = '\n';
    template<typename T>
    concept unsigned_integer = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

    template<unsigned_integer TUInt>
    void print_spec(TUInt val)
    {
        cout_saver{std::cout};
        std::cout << std::hex << "[0x" << val << "]." << newl;
    }

    template<typename T>
    void print_spec(T val) 
    {
        auto saver = cout_saver{std::cout};
        std::cout << "Thing that isn't unsigned integer: [" << std::dec << val << "]." << newl;
    }

    int execute_test_program() noexcept;

    void native_test();

    void log_arg_processing(int argc, char* argv[]);
    	
}

int print_args(int argc, char* argv[]) noexcept;
int execute_log_arg_processing(int argc, char* argv[]) noexcept;

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);
    int ret = print_args(argc, argv);
	if (ret != 0) return ret;
    ret = execute_log_arg_processing(argc, argv);
    if (ret != 0) return ret;
    return cjm::base_test_program::execute_test_program();
}

int print_args(int argc, char* argv[]) noexcept
{
    using namespace cjm::base_test_program;
    std::string exec;
    std::vector<std::string> vec;
	try
	{
        auto [exec_temp, arguments_temp] = 
            cjm::uint128_tests::switches::normalize_and_stringify_console_args(argc, argv);
        exec = std::move(exec_temp);
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

	try
	{
        std::cout << "Will list command line arguments." << newl;
        std::cout << "Executed: [" << exec << "]." << newl;
        size_t this_param = 0;
        const size_t num_params = vec.size();
        std::cout << "Will now print all [" << num_params << "] parameters: " << newl;
		for (const auto& str : vec)
		{
            std::cout << '\t' << "Parameter# " << (++this_param) << " of " << num_params << ": [" << str << "]." << newl;
		}
        std::cout << "Done printing command parameters." << newl;
        std::cout << "Done listing command line arguments." << newl << newl;
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

int execute_log_arg_processing(int argc, char* argv[]) noexcept
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

int cjm::base_test_program::execute_test_program() noexcept
{
    try
    {
        using namespace numerics::uint128_literals;
        using uint128_t = numerics::uint128;
        using testing::cjm_assert;
        using namespace std::string_view_literals;

        std::uint64_t ui = 0xc0de'd00d'fea2'b00bu;
        std::int64_t  i1 = 938'336;
        constexpr auto text = "Hi mom!"sv;
        constexpr auto my_literal = 0xc0de'd00d'fea2'b00b'dead'beef'face'babe_u128;
        constexpr auto my_second_literal = 0xc0de'd00d'fea2'cafe'babe'b00b'face'dad0_u128;
        static_assert(cjm::numerics::concepts::unsigned_integer<uint128_t>);

        {
            auto stream = cjm::string::make_throwing_sstream<char16_t>();
            stream << my_literal;
            std::u16string str = stream.str();
            cjm_assert(!str.empty());
            auto round_tripped = cjm::numerics::uint128::make_from_string(str);
            cjm_assert(round_tripped == my_literal);
        }

        try
        {
            constexpr auto number = 123'456'789;
            auto stream = cjm::string::make_throwing_sstream<char16_t>();
            stream << number;
            int round_tripped = 0;
            stream >> round_tripped;
            cjm_assert(number == round_tripped);
            std::cout << "The char16 conversion actually work in this environment." << newl;
        }
        catch (const std::bad_cast& ex)
        {
            std::cout << "Parsing for REGULAR integers does not exist for utc"
                      <<  " strings.  Ex msg: " << ex.what() << "." << newl;
        }
        catch (const cjm::testing::testing_failure&)
        {
            throw;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "An unexpected exception was thrown while trying"
                      <<" to insert and extract an int with u16 string stream: ["
                      << ex.what() << "]." << newl;
        }

        native_test();

        print_spec(ui);
        print_spec(i1);
        print_spec(text);
        uint128_tests::execute_uint128_tests();

        std::cout << "Here is a cute literal literal: [0x" << std::hex << my_literal << "]." << newl;
        std::cout << "Here the little cutey is as dec: [" << std::dec << my_literal << "]." << newl;
        std::cout << "Another cute literal: [0x" << std::hex << my_second_literal << "]." << newl;
        std::cout << "Another cute literal as dec: [" << std::dec << my_second_literal << "]." << newl;
        std::cout << "Bye now!" << std::endl;
    }
	catch (const std::exception& ex)
	{
        
		try
		{
            std::cerr << "Test program FAILED!  Unhandled exception with message: [" << ex.what() << "].  Program will now terminate." << newl;
            return -1;
		}
		catch (...)
		{
			//naught else can do here....
            return -1;
		}
	}
	catch (...)
	{
        try
        {
            std::cerr << "Test program FAILED!  A non-standard, unhandled exception was thrown.  The program must terminate.";
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


