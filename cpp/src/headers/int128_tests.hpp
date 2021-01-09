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
#include "istream_utils.hpp"

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

    enum class binary_op : unsigned int;
    constexpr size_t binary_op_count = 11;

    class bad_binary_op;

    template<typename TestTypeUi, typename ControlTypeUi>
    concept test_uint_and_control_set = cjm::numerics::concepts::cjm_unsigned_integer<TestTypeUi>
                                        && cjm::numerics::concepts::unsigned_integer<ControlTypeUi>
                                        && std::numeric_limits<TestTypeUi>::digits ==
                                           std::numeric_limits<ControlTypeUi>::digits;

    template<typename TestType = uint128_t , typename ControlType = ctrl_uint128_t>
        requires (test_uint_and_control_set<TestType, ControlType>)
    struct binary_operation;
    using binary_op_u128_t = binary_operation<uint128_t, ctrl_uint128_t>;
    using binary_op_u128_vect_t = std::vector<binary_op_u128_t>;

	template<numerics::concepts::character Char>
    std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char, 
        std::char_traits<Char>>& os, const binary_op_u128_t& op);

    template<numerics::concepts::character Char>
    std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
        std::char_traits<Char>>& is, binary_op_u128_t& op);

	
    template<typename Invocable>
    void execute_test(Invocable test, std::string_view test_name);

    ctrl_uint128_t to_ctrl(uint128_t convert);
    uint128_t to_test(const ctrl_uint128_t& convert);

    constexpr size_t pow_2_arr_size = 63;
    constexpr std::array<std::uint64_t, pow_2_arr_size> get_pow2_arr();
    constexpr std::array<int, pow_2_arr_size> get_pow2_res_arr();

    void execute_trim_tests();
    void execute_ascii_char_interconversions();
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
    void execute_stream_insert_bin_op_test();
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
            static constexpr full_uint_t max_less_one = maximum - full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
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
            static constexpr full_uint_t max_less_one = maximum - full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
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
            static constexpr full_uint_t max_less_one = maximum - full_uint_t{1};;
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
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
            static constexpr full_uint_t max_less_one = maximum - full_uint_t{1};
            static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
            static constexpr full_uint_t one = zero + full_uint_t{1};
            static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
            static constexpr full_uint_t maximum_half_less_one = std::numeric_limits<half_uint_t>::max();
            using half_provider_t = testing_constant_provider<half_uint_t>;
        };
    }
}

namespace std
{
    template<typename TestType, typename ControlType>
        requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
    struct hash<cjm::uint128_tests::binary_operation<TestType, ControlType>>
    {
        std::size_t operator()(const cjm::uint128_tests::binary_operation<TestType, ControlType>& hash_me) noexcept;
    };
}

namespace cjm::uint128_tests
{

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

	
	
    static_assert(!cjm::numerics::has_msc || std::is_unsigned_v<wchar_t>);
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
        "LeftShift"sv, "RightShift"sv, "And"sv, "Or"sv,
            "Xor"sv, "Divide"sv, "Modulus"sv, "Add"sv,
            "Subtract"sv, "Multiply"sv, "Compare"sv};
    constexpr auto op_symbol_lookup = std::array<sv_t, binary_op_count>{
        "<<"sv, ">>"sv, "&"sv, "|"sv,
            "^"sv, "/"sv, "%"sv, "+"sv,
            "-"sv, "*"sv, "<=>"sv};
    constexpr auto op_name_lookup_wide = std::array<wsv_t, binary_op_count>{
        L"LeftShift"sv, L"RightShift"sv, L"And"sv, L"Or"sv,
            L"Xor"sv, L"Divide"sv, L"Modulus"sv, L"Add"sv,
            L"Subtract"sv, L"Multiply"sv, L"Compare"sv};
    constexpr auto op_symbol_lookup_wide = std::array<wsv_t, binary_op_count>{
        L"<<"sv, L">>"sv, L"&"sv, L"|"sv,
            L"^"sv, L"/"sv, L"%"sv, L"+"sv,
            L"-"sv, L"*"sv, L"<=>"sv};
    constexpr auto op_name_lookup_u8 = std::array<u8sv_t, binary_op_count>{
        u8"LeftShift"sv, u8"RightShift"sv, u8"And"sv, u8"Or"sv,
            u8"Xor"sv, u8"Divide"sv, u8"Modulus"sv, u8"Add"sv,
            u8"Subtract"sv, u8"Multiply"sv, u8"Compare"sv};
    constexpr auto op_symbol_lookup_u8 = std::array<u8sv_t, binary_op_count>{
        u8"<<"sv, u8">>"sv, u8"&"sv, u8"|"sv,
            u8"^"sv, u8"/"sv, u8"%"sv, u8"+"sv,
            u8"-"sv, u8"*"sv, u8"<=>"sv};
    constexpr auto op_name_lookup_u16 = std::array<u16sv_t, binary_op_count>{
        u"LeftShift"sv, u"RightShift"sv, u"And"sv, u"Or"sv,
            u"Xor"sv, u"Divide"sv, u"Modulus"sv, u"Add"sv,
            u"Subtract"sv, u"Multiply"sv, u"Compare"sv};
    constexpr auto op_symbol_lookup_u16 = std::array<u16sv_t, binary_op_count>{
        u"<<"sv, u">>"sv, u"&"sv, u"|"sv,
            u"^"sv, u"/"sv, u"%"sv, u"+"sv,
            u"-"sv, u"*"sv, u"<=>"sv};
    constexpr auto op_name_lookup_u32 = std::array<u32sv_t, binary_op_count>{
        U"LeftShift"sv, U"RightShift"sv, U"And"sv, U"Or"sv,
            U"Xor"sv, U"Divide"sv, U"Modulus"sv, U"Add"sv,
            U"Subtract"sv, U"Multiply"sv, U"Compare"sv};
    constexpr auto op_symbol_lookup_u32 = std::array<u32sv_t, binary_op_count>{
        U"<<"sv, U">>"sv, U"&"sv, U"|"sv,
            U"^"sv, U"/"sv, U"%"sv, U"+"sv,
            U"-"sv, U"*"sv, U"<=>"sv};

	
    constexpr sv_t get_op_text(binary_op op);
    constexpr wsv_t get_op_wtext(binary_op op);
    constexpr u8sv_t get_op_u8text(binary_op op);
    constexpr u16sv_t get_op_u16text(binary_op op);
    constexpr u32sv_t get_op_u32text(binary_op op);

    constexpr sv_t get_op_symbol_n(binary_op op);
    constexpr wsv_t get_op_symbol_w(binary_op op);
    constexpr u8sv_t get_op_symbol_u8(binary_op op);
    constexpr u16sv_t get_op_symbol_u16(binary_op op);
    constexpr u32sv_t get_op_symbol_u32(binary_op op);

    template<numerics::concepts::character Char>
    constexpr auto get_op_symbol(binary_op op) -> std::basic_string_view<Char>;

    template<>
    constexpr auto get_op_symbol<char>(binary_op op) -> std::basic_string_view<char>
    {
        return get_op_symbol_n(op);
    }
    template<>
    constexpr auto get_op_symbol<wchar_t>(binary_op op) -> std::basic_string_view<wchar_t>
    {
        return get_op_symbol_w(op);
    }

    template<>
    constexpr auto get_op_symbol<char8_t>(binary_op op) -> std::basic_string_view<char8_t>
    {
        return get_op_symbol_u8(op);
    }

    template<>
    constexpr auto get_op_symbol<char16_t>(binary_op op) -> std::basic_string_view<char16_t>
    {
        return get_op_symbol_u16(op);
    }

    template<>
    constexpr auto get_op_symbol<char32_t>(binary_op op) -> std::basic_string_view<char32_t>
    {
        return get_op_symbol_u32(op);
    }

    binary_op parse_binary_op_symbol(sv_t text);
    binary_op parse_binary_op_symbol(wsv_t text);
    binary_op parse_binary_op_symbol(u8sv_t text);
    binary_op parse_binary_op_symbol(u16sv_t text);
    binary_op parse_binary_op_symbol(u32sv_t text);

    template<numerics::concepts::character Char>
    std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char, std::char_traits<Char>>& os, binary_op op)
    {
        std::basic_string_view<Char> sv = get_op_symbol<std::remove_cvref_t<std::remove_const_t<Char>>>(op);
        os << sv;
        return os;
    }

    template<numerics::concepts::character Char>
    std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
            std::char_traits<Char>>& is, binary_op& op)
    {
        using string_t = std::basic_string<Char>;
        using lsv_t = std::basic_string_view<Char>;
        string_t temp;
        if constexpr (cjm::numerics::has_msc || !cjm::numerics::concepts::utf_character<Char>)
        {
            is >> temp;
        }
        else
        {

            Char c{};
            do
            {
                if (is.good() && !is.eof() && is.peek() != std::char_traits<Char>::eof())
                {
                    is.get(c);
                    if (!is.bad() && !is.fail())
                    {
                        if (!std::isspace<char>(convert_char<Char, char>(c), std::locale("")))
                            temp.push_back(c);
                        else
                            break;
                    }
                }
                else
                    break;
            } while (!is.bad() && !is.fail() && !is.eof());
            if (is.bad() || is.fail())
            {
                return is;
            }
            if (temp.empty())
            {
                is.setstate(std::ios_base::failbit);
                return is;
            }
        }
        op = parse_binary_op_symbol(lsv_t{temp});
        return is;
    }
	
	

    
	
 

    class bad_binary_op final : public std::invalid_argument
    {
    public:
        using stream_t = std::stringstream;
    	bad_binary_op(binary_op op)
		    : invalid_argument(create_message(op)) {}

        using enum_type_t = std::underlying_type_t<binary_op>;
    private:
        static std::string create_message(binary_op op)
        {
            stream_t ss;
            ss  << "The value underlying the binary_op provided ("
        		<< static_cast<enum_type_t>(op)
        		<< "), is not a defined member of the binary_op enum class.";
            return ss.str();
        }
    };
	
    template<typename TestType, typename ControlType>
        requires (test_uint_and_control_set<TestType, ControlType>)
    std::basic_ostream<char>& append_static_assertion(std::basic_ostream<char>& strm, const binary_operation<TestType, ControlType>& bin_op);

    template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
    struct binary_operation final
    {
        using uint_test_t = std::remove_const_t<TestType>;
        using uint_ctrl_t = std::remove_const_t<ControlType>;
        std::size_t hash_value() const noexcept
        {
            std::size_t seed = 0x1FBB0493;
            boost::hash_combine(seed, static_cast<size_t>(m_op));
            boost::hash_combine(seed, std::hash<uint_test_t>{}(m_lhs));
            boost::hash_combine(seed, std::hash<uint_test_t>{}(m_rhs));
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
            validate(op, first_operand, second_operand);
	        if (calculate_now)
	        {
                m_result = perform_calculate_result(first_operand, second_operand, m_op);
	        }
        }
    	
        binary_operation(binary_op op, const uint_test_t& first_operand, 
            const uint_test_t& second_operand) : m_op{ op }, m_lhs{ first_operand },
            m_rhs{ second_operand }, m_result{}
        {
           validate(op, first_operand, second_operand);
        }
        binary_operation(binary_op op, const uint_test_t& first_operand, 
            const uint_test_t& second_operand, const uint_test_t& test_result, 
				const uint_test_t& ctrl_result) : m_op(op), m_lhs(first_operand), m_rhs(second_operand),
				m_result(std::make_pair(test_result, ctrl_result))
        {
            validate(op, first_operand, second_operand);
        }
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

        static void validate(binary_op op, [[maybe_unused]] const uint_test_t& lhs, const uint_test_t& rhs)
        {
            if (op == binary_op::divide || op == binary_op::modulus)
            {
                if (rhs == 0) throw std::invalid_argument{"Zero is not a permissible divisor for a division or modulus operation and may cause undefined behavior with the control type."};
            }
            if (op == binary_op::left_shift || op == binary_op::right_shift)
            {
                size_t test_digits = std::numeric_limits<uint_test_t >::digits;
                size_t ctrl_digits = std::numeric_limits<uint_ctrl_t>::digits;
                uint_ctrl_t rhs_ctrl = to_control(rhs);
                if (rhs_ctrl >= ctrl_digits || rhs >= test_digits)
                {
                    auto ss = cjm::string::make_throwing_sstream<char>();
                    ss  << "The second operand: [" << rhs << "] is not a valid right hand "
                        << "operand for a shift operation: shifting by an amount greater than or "
                        << "equal to [" << test_digits << "] for test value or ["
                        << ctrl_digits << "] for the control value may cause undefined behavior." << newl;
                    throw std::invalid_argument{ss.str()};
                }

            }
        }

        binary_op m_op;
        uint_test_t m_lhs;
        uint_test_t m_rhs;
        std::optional<std::pair<uint_test_t, uint_test_t>> m_result;   

};

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
    std::char_traits<Char>>&os, const binary_op_u128_t& op)
	{
        using char_t = std::remove_const_t<Char>;
        	
        char_t item_separator;
        if constexpr (std::is_same_v<char_t, char>)
        {
            item_separator = ';';
        }
        else
        {
            item_separator = convert_char<char, char_t>(';');
        }

        os << op.op_code() << item_separator;
        os << op.left_operand() << item_separator;
        os << op.right_operand() << item_separator;
        return os;            
	}

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
    std::char_traits<Char>>&is, binary_op_u128_t& op)
	{
        using char_t = std::remove_const_t<Char>;
        using lstr_t = std::basic_string<Char>;
        using lsv_t = std::basic_string<Char>;
        op = binary_op_u128_t{};
        char_t item_separator;
        char_t line_separator;
        if constexpr (std::is_same_v<char_t, char>)
        {
            item_separator = ';';
            line_separator = '\n';
        }
        else
        {
            item_separator = convert_char<char, char_t>(';');
            line_separator = convert_char<char, char_t>('\n');
        }

		if (!is.good() || is.bad() || is.fail() || is.eof() || is.peek() == std::char_traits<char_t>::eof())
		{
            if (is.fail() && !is.bad())
                is.setstate(std::ios_base::failbit | std::ios_base::badbit);
            else
                is.setstate(std::ios_base::failbit);
            return is;
		}

        auto str = lstr_t{};
        std::getline(is, str, line_separator);
		if (str.empty() || is.bad() || is.fail())
		{
            is.setstate(std::ios_base::failbit);
            return is;
		}
		//first string view returned is up to the next delimiter, second string view
		//is remainder of text after the remainder, if any
        auto get_up_to_delim = [](lsv_t txt, char_t delim) -> std::pair<lsv_t, lsv_t>
        {
            if (txt.empty())
                return std::make_pair(lsv_t{}, lsv_t{});

            std::size_t first_delim_idx = 0;
        	for (char_t c : txt)
        	{
        		if (c == delim)
        		{
                      break;
        		}
                ++first_delim_idx;
            }
        	if (first_delim_idx >= txt.size())
        	{
                return std::make_pair(lsv_t{}, lsv_t{});
        	}
            else if (first_delim_idx  == 0)
            {
                return std::make_pair(lsv_t{}, txt.substr(1));
            }
            else
            {   //"2,4"  
                //"<<;1;2"
            	lsv_t first = txt.substr(0, first_delim_idx);
                lsv_t second = first_delim_idx + 1 < txt.size() ? txt.substr(first_delim_idx + 1) : lsv_t{};
                return std::make_pair(first, second);
            }
        };
        auto temp = std::array<lsv_t, 3>{};
		lsv_t line = str;
        lsv_t remainder = line;
        int added = 0;
		while (added < 3 && !remainder.empty())
		{
            auto [symbol, remainder_temp] = get_up_to_delim(remainder, item_separator);
			if (symbol.empty())
			{
                is.setstate(std::ios_base::failbit);
                return is;
			}
            temp[added++] = symbol;
            remainder = remainder_temp;
		}
        if (added == 3 && std::all_of(temp.cbegin(), temp.cend(), [](const lsv_t& t) -> bool {return !t.empty(); }))
        {
            auto op_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[0]);
            auto lhs_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[1]);
            auto rhs_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[2]);
            binary_op op_code = parse_binary_op_symbol(op_text);
            uint128_t lhs = uint128_t::make_from_string(lhs_text);
            uint128_t rhs = uint128_t::make_from_string(rhs_text);
            op= binary_op_u128_t{ op_code, lhs, rhs };
            return is;
        }
        is.setstate(std::ios_base::failbit);
        return is;		
	}
	
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


constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_op_text(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_name_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_op_wtext(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_name_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_op_u8text(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_name_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_op_u16text(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_name_lookup_u16[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_op_u32text(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_name_lookup_u32[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_op_symbol_n(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_symbol_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_op_symbol_w(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_symbol_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_op_symbol_u8(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_symbol_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_op_symbol_u16(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_symbol_lookup_u16[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_op_symbol_u32(binary_op op)
{
    if (op < first_op || op > last_op) throw bad_binary_op{ op };
    return op_symbol_lookup_u32[static_cast<unsigned>(op)];
}


constexpr std::array<std::uint64_t, cjm::uint128_tests::pow_2_arr_size> cjm::uint128_tests::get_pow2_arr()
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
template<typename TestType, typename ControlType>
    requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
std::size_t std::hash<cjm::uint128_tests::binary_operation<TestType, ControlType>>::operator()(const cjm::uint128_tests::binary_operation<TestType, ControlType>& hash_me) noexcept
{
    return hash_me.hash_value();
}
#endif //INT128_INT128_TESTS_HPP
#include "int128_tests.inl"
