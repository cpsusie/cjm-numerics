#ifndef INT128_INT128_TESTS_HPP
#define INT128_INT128_TESTS_HPP
#include "numerics.hpp"
#include "uint128.hpp"
#include "cjm_string.hpp"
#include "testing.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <string>
#include <sstream>
#include <limits>
#include <type_traits>
#include <cstdint>
#include <boost/io/ios_state.hpp>
#include <exception>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>

namespace cjm::uint128_tests
{
    using ctrl_uint128_t = boost::multiprecision::uint128_t;
    using uint128_t = numerics::uint128;
    using namespace numerics::uint128_literals;
    using namespace numerics::literals;
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    using testing::cjm_assert;
    using testing::cjm_deny;
    using std::cout;
    constexpr auto newl = '\n';
    using cout_saver = boost::io::ios_flags_saver;
    using sv_t = std::string_view;
    constexpr size_t binary_op_count = 11;
    enum class binary_op : unsigned int
    {
        left_shift = 0,
        right_shift,
        bw_and,
        bw_or,
        bw_xor,

        divide,
        modulus,
        add,
        subtract,
        multiply,

        compare
    };
    constexpr std::array<sv_t, binary_op_count> op_name_lookup =
        std::array<sv_t, binary_op_count>{
        "LeftShift"sv, "RightShift"sv,
            "And"sv, "Or"sv,
            "Xor"sv, "Divide"sv,
            "Modulus"sv, "Add"sv,
            "Subtract"sv, "Multiply"sv,
            "Compare"sv};

    struct binary_operation;
    class cjm_helper_rgen;
	
    template<typename Invocable>
    void execute_test(Invocable test, std::string_view test_name);

    ctrl_uint128_t to_ctrl(uint128_t convert);
    uint128_t to_test(const ctrl_uint128_t& convert);

    constexpr size_t pow_2_arr_size = 63;
    constexpr std::array<std::uint64_t, pow_2_arr_size> get_pow2_arr()
    {
        std::uint64_t current = 1;
        
        std::array<std::uint64_t, pow_2_arr_size> ret{};
    	for (size_t idx = 0; idx < ret.size(); ++idx)
    	{
            ret[idx] = current;
            current <<= 1;
    	}
        return ret;
    }
    constexpr std::array<int, pow_2_arr_size> get_pow2_res_arr()
    {
        using namespace cjm::numerics::literals;
        constexpr std::array<std::uint64_t, pow_2_arr_size> work_on_me = get_pow2_arr();
        std::array<int, pow_2_arr_size> ret{};
    	for (auto i = 0_szt; i < work_on_me.size(); ++i)
    	{
            ret[i] = cjm::numerics::internal::fls_slow(work_on_me[i]);
    	}
        return ret;
    }	
	
	
    void execute_uint128_tests();
    void execute_basic_test_one();
    void execute_string_parse_test();
    void execute_basic_multiplication_test();
    void test_fls();
    void print_uint128_eval_mode();
    void test_interconversion(const ctrl_uint128_t& control, uint128_t test);

    struct binary_operation final
    {
        using uint128_t = cjm::numerics::uint128;
        friend std::size_t hash_value(const binary_operation& obj)
        {

            std::size_t seed = 0x1FBB0493;
            boost::hash_combine(seed, static_cast<size_t>(obj.m_op));
            boost::hash_combine(seed, std::hash<uint128_t>{}(obj.m_lhs));
            boost::hash_combine(seed, std::hash<uint128_t>{}(obj.m_rhs));
            return seed;
        }

        friend bool operator<(const binary_operation& lhs, const binary_operation& rhs)
        {
            if (lhs.m_op < rhs.m_op)
                return true;
            if (rhs.m_op < lhs.m_op)
                return false;
            if (lhs.m_rhs < rhs.m_rhs)
                return true;
            if (rhs.m_rhs < lhs.m_rhs)
                return false;
            return lhs.m_lhs < rhs.m_lhs;
        }

        friend bool operator<=(const binary_operation& lhs, const binary_operation& rhs) { return !(rhs < lhs); }

        friend bool operator>(const binary_operation& lhs, const binary_operation& rhs) { return rhs < lhs; }

        friend bool operator>=(const binary_operation& lhs, const binary_operation& rhs) { return !(lhs < rhs); }

        friend bool operator==(const binary_operation& lhs, const binary_operation& rhs)
        {
            return lhs.m_op == rhs.m_op
                && lhs.m_rhs == rhs.m_rhs
                && lhs.m_lhs == rhs.m_lhs;
        }

        friend bool operator!=(const binary_operation& lhs, const binary_operation& rhs) { return !(lhs == rhs); }

        [[nodiscard]] binary_op op_code() const noexcept { return m_op; }
        [[nodiscard]] uint128_t left_operand() const noexcept { return m_lhs; }
        [[nodiscard]] uint128_t right_operand() const noexcept { return m_rhs; }
        [[nodiscard]] std::optional<uint128_t> result() const noexcept { return m_result; }
        [[nodiscard]] bool has_result() const noexcept { return m_result.has_value(); }
        [[nodiscard]] bool has_correct_result() const
        {
            return m_result.has_value() && binary_operation::perform_calculate_result(m_lhs, m_rhs, m_op) == m_result.value();
        }

        binary_operation() noexcept;
        binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand, bool calculate_now);
        binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand);
        binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand, uint128_t result);
        binary_operation(const binary_operation& other) noexcept = default;
        binary_operation(binary_operation&& other) noexcept = default;
        binary_operation& operator=(const binary_operation& other) noexcept = default;
        binary_operation& operator=(binary_operation&& other) noexcept = default;
        ~binary_operation() = default;

        void calculate_result() { do_calculate_result(); }

    private:

        bool do_calculate_result();
        static uint128_t perform_calculate_result(uint128_t lhs, uint128_t rhs, binary_op op) noexcept;

        binary_op m_op;
        uint128_t m_lhs;
        uint128_t m_rhs;
        std::optional<uint128_t> m_result;

    };
}

template<typename Invocable>
void cjm::uint128_tests::execute_test(Invocable test, std::string_view test_name)
{
    cout_saver o_saver{cout};
    cout_saver e_saver{std::cerr};

    cout << "Beginning test: [" << test_name << "]:" << newl;
    try
    {
        test();
        cout << "Test [" << test_name << "] PASSED" << newl << newl;
    }
    catch (const std::exception& ex)
    {
        std::stringstream ss;
        ss << "Test [" << test_name << "] failed with exception message: [" << ex.what() << "]." << newl;
        std::cerr << ss.str();
        throw;
    }
}




#endif //INT128_INT128_TESTS_HPP
