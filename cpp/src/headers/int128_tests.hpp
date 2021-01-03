#ifndef INT128_TESTS_HPP_
#define INT128_TESTS_HPP_
#include <iostream>
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
#include <random>
#include <algorithm>
#include <vector>
#include<functional>
#include <memory>
#include <optional>

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

    namespace u128_testing_constant_providers
    {
        namespace concepts
        {
        	//Must be a cjm_unsigned_integer or a built-in, unsigned integer that is no larger than 64 bits.
            template<typename T>
            concept supports_testing_constant_provider =
                //EITHER: (integer AND unsigned AND size <= 64)
                (std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed && std::is_fundamental_v<T> &&
                        sizeof(T) <= sizeof(std::uint64_t)) // OR
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
    {   //IF YOU EDIT THIS, MAKE SURE YOU EDIT THE CONSTANTS BELOW AND KEEP RELATED GROUPS CONSECUTIVE!
        left_shift = 0,
        right_shift = 1,

        bw_and = 2,
        bw_or = 3,
        bw_xor = 4,

        divide = 5,
        modulus = 6,

        add = 7,
        subtract = 8,
        multiply = 9,

        compare = 10
    };

    constexpr binary_op first_op = binary_op::left_shift;
    constexpr binary_op last_op = binary_op::compare;

    constexpr binary_op first_shift_op = binary_op::left_shift;
    constexpr binary_op last_shift_op = binary_op::right_shift;

    constexpr binary_op first_bw_op = binary_op::bw_and;
    constexpr binary_op last_bw_op = binary_op::bw_xor;

    constexpr binary_op first_divmod_op = binary_op::divide;
    constexpr binary_op last_divmod_op = binary_op::modulus;

    constexpr binary_op first_add_sub_mul_op = binary_op::add;
    constexpr binary_op last_add_sub_mul_op = binary_op::multiply;
	
	
    constexpr auto op_name_lookup = std::array<sv_t, binary_op_count>{
        "LeftShift"sv, "RightShift"sv,"And"sv, "Or"sv,
            "Xor"sv, "Divide"sv,"Modulus"sv, "Add"sv,
            "Subtract"sv, "Multiply"sv,"Compare"sv};
    constexpr auto op_symbol_lookup = std::array<sv_t, binary_op_count>{
    	"<<"sv, ">>"sv, "&"sv, "|"sv,
    	"^"sv, "/"sv, "%"sv, "+"sv,
    	"-"sv, "*"sv, "<=>"sv};
		
	
    template<typename TestType = uint128_t , typename ControlType = ctrl_uint128_t>
            requires (test_uint_and_control_set<TestType, ControlType>)
    struct binary_operation;
    
	
    template<typename Invocable>
    void execute_test(Invocable test, std::string_view test_name);

    ctrl_uint128_t to_ctrl(uint128_t convert);
    uint128_t to_test(const ctrl_uint128_t& convert);

    constexpr size_t pow_2_arr_size = 63;
    constexpr std::array<std::uint64_t, pow_2_arr_size> get_pow2_arr();
    constexpr std::array<int, pow_2_arr_size> get_pow2_res_arr();

    void execute_div_mod_zero_dividend_nonzero_divisor_tests();
    void execute_div_mod_by_zero_tests();
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
    void execute_gen_comp_ops_test();

    template<typename TestType, typename ControlType>
    requires (test_uint_and_control_set<TestType, ControlType>)
        std::basic_ostream<char>& append_static_assertion(std::basic_ostream<char>& strm, const binary_operation<TestType, ControlType>& bin_op);
	
    template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
    struct binary_operation final
    {
        using uint_test_t = std::remove_const_t<TestType>;
        using uint_ctrl_t = std::remove_const_t<ControlType>;
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
    	
        binary_operation(binary_op op, const uint_test_t& first_operand, 
            const uint_test_t& second_operand) : m_op{ op }, m_lhs{ first_operand },
            m_rhs{ second_operand }, m_result{} {}
        binary_operation(binary_op op, const uint_test_t& first_operand, 
            const uint_test_t& second_operand, const uint_test_t& test_result, 
				const uint_test_t& ctrl_result) : m_op(op), m_lhs(first_operand), m_rhs(second_operand), m_result(std::make_pair(test_result, ctrl_result)) {}
        binary_operation(const binary_operation& other) noexcept = default;
        binary_operation(binary_operation&& other) noexcept = default;
        binary_operation& operator=(const binary_operation& other) noexcept = default;
        binary_operation& operator=(binary_operation&& other) noexcept = default;
        ~binary_operation() = default;

        void calculate_result() { do_calculate_result(); }

    private:

        static auto to_control(const uint_test_t& test)-> uint_ctrl_t
        {
            uint_ctrl_t ctrl = test.high_part();
            ctrl <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
            ctrl |= test.low_part();
            return ctrl;
        }


        static uint_test_t to_test(const uint_ctrl_t& ctrl)
        {
            uint_test_t high = static_cast<typename uint_test_t::int_part>(ctrl >> std::numeric_limits<typename uint_test_t::int_part>::digits);
            high <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
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
        static std::pair<uint_test_t, uint_test_t> perform_calculate_result(const uint_test_t& lhs, const uint_test_t& rhs, binary_op op) 
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
	template <typename TestType, typename ControlType>
	requires (test_uint_and_control_set<TestType, ControlType>)
	std::basic_ostream<char>& append_static_assertion(
	    std::basic_ostream<char>& strm, const binary_operation<TestType, ControlType>& bin_op)
	{
        using uint_test_t = typename binary_operation<TestType, ControlType>::uint_test_t;
        auto saver = cout_saver{ strm };
        strm << "static_assert(0x"
            << std::hex << std::setfill('0')
            << bin_op.left_operand() << "_u" << std::dec
            << std::numeric_limits<uint_test_t>::digits
            << " " << op_symbol_lookup[static_cast<unsigned>(bin_op.op_code())]
            << " 0x" << std::hex << std::setfill('0')
            << bin_op.right_operand() << "_u" << std::dec
            << std::numeric_limits<uint_test_t>::digits << " == ";
        if (!bin_op.has_correct_result())
        {
            strm << "UNKNOWN OR INCORRECT VALUE"  << newl;
        }
        else
        {
            auto result = bin_op.result();
            auto value = result.value().first;
            if (bin_op.op_code() == binary_op::compare)
            {
            	if (value == 0_u128)
                    strm << "std::strong_ordering::equal";
                else if (value == 1_u128)
					strm << "std::strong_ordering::greater";
                else
					strm << "std::strong_ordering::less";
            }
            else
            {
                strm << "0x" << std::hex << std::setfill('0') << value << "_u" << std::dec
                    << std::numeric_limits<uint_test_t>::digits;
            }
        }
        strm << ");" << newl;
	    return strm;
	}
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



constexpr std::array<std::uint64_t, cjm::uint128_tests::pow_2_arr_size> cjm::uint128_tests::get_pow2_arr()
{
    std::uint64_t current = 1;
    std::array<std::uint64_t, cjm::uint128_tests::pow_2_arr_size> ret{};
    for (size_t idx = 0; idx < ret.size(); ++idx)
    {
        ret[idx] = current;
        current <<= 1;
    }
    return ret;
}

constexpr std::array<int, cjm::uint128_tests::pow_2_arr_size> cjm::uint128_tests::get_pow2_res_arr()
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

namespace cjm::uint128_tests::generator
{
    static_assert(sizeof(std::uintptr_t) >= 8 || sizeof(std::uintptr_t) == 4,
        "Only 32 and 64 bit systems are supported.");
    namespace internal
    {

        class rgen_impl;
    }
    class rgen;
}

namespace std
{
    template<>
    struct hash<cjm::uint128_tests::generator::rgen>
    {
        [[nodiscard]] size_t operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept;

    };
}
namespace cjm::uint128_tests
{
    template<typename TestType, typename ControlType>
    requires (test_uint_and_control_set<TestType, ControlType>)
        struct binary_operation;
    using uint128_t = cjm::numerics::uint128;

}
namespace cjm::uint128_tests::generator
{
    using uint128_t = numerics::uint128;
    using ctrl_uint128_t = boost::multiprecision::uint128_t;
    enum class binary_op_category
    {
        any,
        bitwise,
        bitshift,
        add_subtract_multiply,
        div_mod,
        compare,
    };

    namespace concepts
    {
        template<typename UnsignedInteger>
        concept up_to_ui128 = std::numeric_limits<UnsignedInteger>::is_integer &&
            std::numeric_limits<UnsignedInteger>::digits <= 128 && (!std::numeric_limits<UnsignedInteger>::is_signed);

        template<typename Invocable>
        concept binary_op_producer = requires (Invocable func, const rgen & gen, binary_operation<uint128_t, ctrl_uint128_t> op)
        {
            {op = func(gen) };

        };
    }


	
    binary_operation<cjm::uint128_tests::uint128_t,
                     cjm::uint128_tests::ctrl_uint128_t> create_compare_op(const rgen& rgen);
    binary_operation<uint128_t, ctrl_uint128_t> create_shift_op(const rgen& rgen);
    binary_operation<uint128_t, ctrl_uint128_t> create_bw_op(const rgen& rgen);
    binary_operation<uint128_t, ctrl_uint128_t>
        create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

    std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
        create_division_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);

    binary_operation<uint128_t, ctrl_uint128_t>
        create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

    std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
        create_modulus_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);

    template<concepts::up_to_ui128 UnsignedInteger>
    uint128_t create_random_in_range(const rgen& rgen);

    template<concepts::up_to_ui128 UnsignedInteger>
    int create_shift_param_for_type(const rgen& rgen);

	template<concepts::binary_op_producer OpFactory>
    std::vector<binary_operation<uint128_t, ctrl_uint128_t>> create_many_ops(OpFactory factory, const rgen& gen, size_t num_ops);

    bool operator==(const rgen& lhs, const rgen& rhs) noexcept;
    bool operator!=(const rgen& lhs, const rgen& rhs) noexcept;
    uint128_t create_random_in_range(const rgen& rgen, size_t byte_limit);

    class rgen final
    {
    public:
        [[nodiscard]] bool good() const { return static_cast<bool>(m_gen); }
        rgen();
        rgen(const rgen& other) = delete;
        rgen(rgen&& other) noexcept;
        rgen& operator=(const rgen& other) = delete;
        rgen& operator=(rgen&& other) noexcept;
        ~rgen() = default;

        friend bool operator==(const rgen& lhs, const rgen& rhs) noexcept;
        friend bool operator!=(const rgen& lhs, const rgen& rhs) noexcept;
        friend struct std::hash<rgen>;
        explicit operator bool() const { return good(); }
        bool operator!() const { return !good(); }

        [[nodiscard]] binary_op random_op(binary_op_category category) const;
        [[nodiscard]] std::vector<std::uint8_t> generate(size_t bytes) const;
        [[nodiscard]] int generate_shift_param(int binary_digits) const;
    private:
        explicit rgen(bool) noexcept;
        std::unique_ptr<internal::rgen_impl> m_gen;
    };


    template<concepts::up_to_ui128 UnsignedInteger>
    uint128_t create_random_in_range(const rgen& rgen)
    {
        assert(rgen.good());
        using byte_array = typename uint128_t::byte_array;
        auto vec = rgen.generate(sizeof(UnsignedInteger));
        auto arr = byte_array{};
        assert(vec.size() <= arr.size());
        std::copy(vec.cbegin(), vec.cend(), arr.begin());
        return uint128_t::make_from_bytes_little_endian(arr);
    }

    template <concepts::up_to_ui128 UnsignedInteger>
    int create_shift_param_for_type(const rgen& rgen)
    {
        return rgen.generate_shift_param(std::numeric_limits<UnsignedInteger>::digits);
    }

    template<concepts::binary_op_producer OpFactory>
    std::vector<binary_operation<uint128_t, ctrl_uint128_t>> create_many_ops(OpFactory factory, const rgen& gen, size_t num_ops)
    {
        auto vec = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
    	if (num_ops > 0)
    	{
            vec.reserve(num_ops);
    	}
    	while (vec.size() < num_ops)
    	{
            vec.emplace_back(factory(gen));
    	}
        return vec;
    }

    //static_assert(std::is_same_v<twister_t, std::mt19937_64>);

    //inline std::mt19937_64 init_twister()
    //{
    //	std::random_device rd;
    //	auto seed = rd();
    //	return std::mt19937_64{ seed };
    //}

}

#endif //INT128_INT128_TESTS_HPP
#include "int128_tests.inl"
