#ifndef INT128_INT128_TESTS_HPP
#define INT128_INT128_TESTS_HPP
#include "numerics.hpp"
#include "uint128.hpp"
#include "cjm_numeric_concepts.hpp"
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
#include <utility>
#include <boost/multiprecision/cpp_int.hpp>
#include <concepts>
#include <cstdint>
#include "int128_tests.hpp"

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

    template<typename TestTypeUi, typename ControlTypeUi>
    concept test_uint_and_control_set = cjm::numerics::concepts::cjm_unsigned_integer<TestTypeUi>
        && cjm::numerics::concepts::unsigned_integer<ControlTypeUi>
        && std::numeric_limits<TestTypeUi>::digits == std::numeric_limits<ControlTypeUi>::digits;

       /* std::numeric_limits<TestTypeUi>::digits == std::numeric_limits<ControlTypeUi>::digits && 
        (std::numeric_limits<TestTypeUi>::is_integer && !std::numeric_limits<TestTypeUi>::is_signed) && 
        (std::numeric_limits<ControlTypeUi>::is_integer && !std::numeric_limits<ControlTypeUi>::is_signed);*/
    namespace u128_testing_constant_providers
    {

        namespace concepts
        {
        	//Must be a cjm_unsigned_integer or a built-in, unsigned integer that is no larger than 64 bits.
            template<typename T>
            concept supports_testing_constant_provider =
                //EITHER: (integer AND unsigned AND size <= 64) OR
                (std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed && std::is_fundamental_v<T> && sizeof(T) <= sizeof(std::uint64_t))
        		//complies with cjm_unsigned_integer<T>
        	|| cjm::numerics::concepts::cjm_unsigned_integer<T>;

        }

        template<concepts::supports_testing_constant_provider T>
        struct testing_constant_provider;
    	
        template<>
        struct testing_constant_provider<std::uint8_t>
        {
            using full_uint_t = std::uint8_t;
            using half_uint_t = std::uint8_t;

            static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
            static constexpr full_uint_t max_less_one = maximum -  full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero +  full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };

        template<>
        struct testing_constant_provider<std::uint16_t>
        {
            using full_uint_t = std::uint16_t;
            using half_uint_t = std::uint8_t;

            static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
            static constexpr full_uint_t max_less_one = maximum -  full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero +  full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };

        template<>
        struct testing_constant_provider<std::uint32_t>
        {
            using full_uint_t = std::uint32_t;
            using half_uint_t = std::uint16_t;

            static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
            static constexpr full_uint_t max_less_one = maximum - full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };

        template<>
        struct testing_constant_provider<std::uint64_t> final
        {
            using full_uint_t = std::uint64_t;
            using half_uint_t = std::uint32_t;

            static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
            static constexpr full_uint_t max_less_one = maximum -  full_uint_t{1};;
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero +  full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };

        template<concepts::supports_testing_constant_provider T>
        struct testing_constant_provider final
        {
            using full_uint_t = std::remove_const_t<T>;
            using half_uint_t = typename T::int_part;

            static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
            static constexpr full_uint_t max_less_one = maximum - full_uint_t {1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };
    }

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
    template<typename TestType = uint128_t , typename ControlType = ctrl_uint128_t>
            requires (test_uint_and_control_set<TestType, ControlType>)
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
            ret[i] = cjm::numerics::internal::fls_default(work_on_me[i]);
    	}
        return ret;
    }	
	
	
    void execute_uint128_tests();
    void execute_basic_test_one();
    void execute_string_parse_test();
    void execute_basic_multiplication_test();
    void test_fls();
    void print_uint128_eval_mode();
    void print_constexpr_bitcast_available();
    void print_cpp20_bitops_available();
    void print_builtin_uint128_data_if_present();
    void test_interconversion(const ctrl_uint128_t& control, uint128_t test);
    void execute_builtin_u128fls_test_if_avail();
    void execute_first_bin_op_test();
    template<typename TestType, typename ControlType>
        requires (test_uint_and_control_set<TestType, ControlType>)
    struct binary_operation final
    {
        using uint_test_t = TestType;
        using uint_ctrl_t = ControlType;
        friend std::size_t hash_value(const binary_operation& obj)
        {

            std::size_t seed = 0x1FBB0493;
            boost::hash_combine(seed, static_cast<size_t>(obj.m_op));
            boost::hash_combine(seed, std::hash<uint_test_t>{}(obj.m_lhs));
            boost::hash_combine(seed, std::hash<uint_test_t>{}(obj.m_rhs));
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
        friend std::weak_ordering operator <=>(const binary_operation& lhs, const binary_operation& rhs) = default;
    	
        [[nodiscard]] binary_op op_code() const noexcept { return m_op; }
        [[nodiscard]] const uint_test_t& left_operand() const noexcept { return m_lhs; }
        [[nodiscard]] const uint_test_t& right_operand() const noexcept { return m_rhs; }
        [[nodiscard]] const std::optional<std::pair<uint_test_t, uint_test_t>>& result() const noexcept { return m_result; }
        [[nodiscard]] bool has_result() const noexcept { return m_result.has_value(); }
        [[nodiscard]] bool has_correct_result() const
        {
            return m_result.has_value() && binary_operation::perform_calculate_result(m_lhs, m_rhs, m_op) == m_result.value() && m_result->first == m_result->second;
        }

        binary_operation() noexcept : m_op(), m_lhs(), m_rhs(), m_result() {}
        binary_operation(binary_op op, const uint_test_t& first_operand, const uint_test_t& second_operand, bool calculate_now) : m_op(op), m_lhs(first_operand), m_rhs(second_operand), m_result()
        {
	        if (calculate_now)
	        {
                m_result = perform_calculate_result(first_operand, second_operand, m_op);
	        }
        }
        binary_operation(binary_op op, const uint_test_t& first_operand, const uint_test_t& second_operand) : binary_operation(op, first_operand, second_operand, false){}
        binary_operation(binary_op op, const uint_test_t& first_operand, const uint_test_t& second_operand, const uint_test_t& test_result, const uint_test_t& ctrl_result) : m_op(op), m_lhs(first_operand), m_rhs(second_operand), m_result(std::make_pair(test_result, ctrl_result)) {}
        binary_operation(const binary_operation& other) noexcept = default;
        binary_operation(binary_operation&& other) noexcept = default;
        binary_operation& operator=(const binary_operation& other) noexcept = default;
        binary_operation& operator=(binary_operation&& other) noexcept = default;
        ~binary_operation() = default;

        void calculate_result() { do_calculate_result(); }

    private:

    	
        static uint_ctrl_t to_control(const uint_test_t& test)
        {
            uint_ctrl_t ctrl = test.high_part();
            ctrl <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
            ctrl |= test.low_part();
            return ctrl;
        	
        }
        static uint_test_t to_test(const uint_ctrl_t& ctrl)
        {
            uint_test_t high = static_cast<typename uint_test_t::int_part>(ctrl >> std::numeric_limits<typename uint_test_t::int_part>::digits);
            uint_test_t low = static_cast<typename uint_test_t::int_part>(ctrl);
            return high | low;
        }

        bool do_calculate_result()
        {
            auto result = perform_calculate_result(m_lhs, m_rhs, m_op);
            const bool changed_value = (!m_result.has_value() || *m_result != result);
            m_result = result;
            return changed_value;
        }
        static std::pair<uint_test_t, uint_test_t> perform_calculate_result(const uint_test_t& lhs, const uint_test_t& rhs, binary_op op) noexcept
        {
            assert(static_cast<size_t>(op) < op_name_lookup.size());
            uint_test_t ret_tst = 0;
            uint_ctrl_t ret_ctrl = 0;
            uint_ctrl_t lhs_ctrl = to_control(lhs);
            uint_ctrl_t rhs_ctrl = to_control(rhs);
        	
            switch (op)
            {
            case binary_op::left_shift:
                ret_tst = lhs << static_cast<int>(rhs);
                ret_ctrl = lhs_ctrl << static_cast<int>(rhs);
                break;
            case binary_op::right_shift:
                ret_tst = lhs >> static_cast<int>(rhs_ctrl);
                ret_ctrl = lhs_ctrl >> static_cast<int>(rhs_ctrl);
                break;
            case binary_op::bw_and:
                ret_tst = lhs & rhs;
                ret_ctrl = lhs_ctrl & rhs_ctrl;
                break;
            case binary_op::bw_or:
                ret_tst = lhs | rhs;
                ret_ctrl = lhs_ctrl | rhs_ctrl;
            	break;
            case binary_op::bw_xor:
                ret_tst = lhs ^ rhs;
                ret_ctrl = lhs_ctrl ^ rhs_ctrl;
                break;
            case binary_op::divide:
                ret_tst = lhs / rhs;
                ret_ctrl = lhs_ctrl / rhs_ctrl;
                break;
            case binary_op::modulus:
                ret_tst = lhs % rhs;
                ret_ctrl = lhs_ctrl % rhs_ctrl;
                break;
            case binary_op::add:
                ret_tst = lhs + rhs;
                ret_ctrl = lhs_ctrl + rhs_ctrl;
                break;
            case binary_op::subtract:
                ret_tst = lhs - rhs;
                ret_ctrl = lhs_ctrl - rhs_ctrl;
                break;
            case binary_op::multiply:
                ret_tst = lhs * rhs;
                ret_ctrl = lhs_ctrl * rhs_ctrl;
                break;
            case binary_op::compare:
                if (lhs_ctrl == rhs_ctrl)
                {
                    ret_ctrl = 0;
                }
            	else
            	{
                    ret_ctrl = lhs_ctrl > rhs_ctrl ? 1 : std::numeric_limits<uint_ctrl_t>::max();
            	}
                if (lhs == rhs)
                {
                    ret_tst = 0;
                }
                else
                {
                    ret_tst = lhs > rhs ? 1 : u128_testing_constant_providers::testing_constant_provider<uint_test_t>::maximum;
                }
                break;
            }
            return std::make_pair(ret_tst, to_test(ret_ctrl));
        }

        binary_op m_op;
        uint_test_t m_lhs;
        uint_test_t m_rhs;
        std::optional<std::pair<uint_test_t, uint_test_t>> m_result;

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
#include "int128_tests.inl"
