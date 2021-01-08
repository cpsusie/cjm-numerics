#include "int128_tests.hpp"

namespace
{
    using uint128_t = cjm::numerics::uint128;
    using uint128_ctrl = boost::multiprecision::uint128_t;
    constexpr std::string_view bool_to_yes_no(bool value) noexcept
    {
        using namespace std::string_view_literals;
        return value ? "YES"sv : "NO"sv;
    }
}

cjm::uint128_tests::ctrl_uint128_t cjm::uint128_tests::to_ctrl(uint128_t convert) 
{
    ctrl_uint128_t ret = convert.high_part();
    ret <<= 64;
    ret |= convert.low_part();
    return ret;
}

cjm::uint128_tests::uint128_t cjm::uint128_tests::to_test(const ctrl_uint128_t& convert) 
{
	const std::uint64_t low_part = static_cast<std::uint64_t>(convert);
	const std::uint64_t high_part = static_cast<std::uint64_t>(convert >> 64);
    return uint128_t::make_uint128(high_part, low_part);
}

void cjm::uint128_tests::execute_ascii_char_interconversions()
{
    for (int i = 0; i <= std::numeric_limits<char>::max(); ++i)
    {
        char c = static_cast<char>(i);
        wchar_t wc = convert_char<char, wchar_t>(c);
        char8_t u8c = convert_char<char, char8_t>(c);
        char16_t u16c = convert_char<char, char16_t>(c);
        char32_t u32c = convert_char<char, char32_t>(c);

        cjm_assert(convert_char<wchar_t, char>(wc) == c);
        cjm_assert(convert_char<char8_t, char>(u8c) == c);
        cjm_assert(convert_char<char16_t, char>(u16c) == c);
        cjm_assert(convert_char<char32_t, char>(u32c) == c);

    }
}

void cjm::uint128_tests::execute_div_mod_zero_dividend_nonzero_divisor_tests()
{
    constexpr auto dividend = 0_u128;
    constexpr auto divisor = 0xd00d_u128;
    static_assert(0_u128 <=> 1_u128 == std::strong_ordering::less);
    cjm_assert(dividend / divisor == 0_u128);
    cjm_assert(dividend % divisor == 0_u128);
    auto divmod_res = uint128_t::div_mod(dividend, divisor);
    cjm_assert(divmod_res.quotient == 0 && divmod_res.remainder == 0);
    auto res = uint128_t::try_div_mod(dividend, divisor);
    cjm_assert(res.has_value() && res->quotient == 0 && res->remainder == 0);
}

void cjm::uint128_tests::execute_div_mod_by_zero_tests()
{
    using namespace testing;
	
    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto quotient = dividend / divisor;
       std::cerr << "You should never see this value: [" << quotient << "]." << newl;
    });

    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto remainder = dividend % divisor;
        std::cerr << "You should never see this value: [" << remainder << "]." << newl;
    });

    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto result = uint128_t::div_mod(dividend, divisor);
        std::cerr << "You should never see this value: [Quotient: " << result.quotient << "; remainder: " << result.remainder << "]." << newl;
    });
    cjm_assert(uint128_t::try_div_mod(0xd00d_u128, 0_u128) == std::nullopt);
	
}

void cjm::uint128_tests::execute_uint128_tests()
{
    using tconst_prov_t [[maybe_unused]] = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
    static_assert(tconst_prov_t::maximum + tconst_prov_t::one == tconst_prov_t::zero);
        	
    constexpr auto two_fifty_five = 0xff_u128;
	constexpr auto all_at_ends = 0xff00'0000'0000'0000'0000'0000'0000'0000_u128;
    static_assert(((two_fifty_five << (15 * 8)) == all_at_ends) && ((all_at_ends >> (15 * 8)) == two_fifty_five));
    cout << "ENVIRONMENT DATA: " << newl;
    print_constexpr_bitcast_available();
    print_uint128_eval_mode();
    print_cpp20_bitops_available();
    print_builtin_uint128_data_if_present();
    cout << "END ENVIRONMENT DATA" << newl << newl;
    execute_test(execute_basic_test_one, "basic_test_one"sv);
    execute_test(execute_string_parse_test, "string_parse_text"sv);
    execute_test(execute_basic_multiplication_test, "basic_multiplication_test"sv);
    execute_test(test_fls, "test_fls"sv);
    execute_test(execute_builtin_u128fls_test_if_avail, "builtin_u128fls_test_if_avail"sv);
    execute_test(execute_first_bin_op_test, "first_bin_op_test"sv);
    execute_test(execute_div_mod_by_zero_tests, "div_mod_zero_tests"sv);
    execute_test(execute_div_mod_zero_dividend_nonzero_divisor_tests, "div_mod_zero_dividend_nonzero_divisor_tests"sv);
    execute_test(execute_gen_comp_ops_test, "gen_comp_ops_test"sv);
    execute_test(execute_ascii_char_interconversions, "ascii_char_interconversions"sv);
    execute_test(execute_trim_tests, "trim_tests"sv);
    execute_test(execute_stream_insert_bin_op_test, "stream_insert_bin_op_test"sv);
	cout_saver saver{cout};
    cout << "All tests PASSED." << newl;
}

void cjm::uint128_tests::execute_basic_test_one()
{
    constexpr uint128_t big = 0xc0de'd00d'fea2'b00b'c0de'd00d'fea2'b00b_u128;
    cout_saver saver{cout};
    cout << std::hex << "Printing hex: [0x" <<  big << "]." << newl;
    cout << std::dec << "Printing dec: [" << big << "]." << newl;
}


void cjm::uint128_tests::execute_string_parse_test()
{
    constexpr uint128_t test_hex = 0xc0de'd00d'face'cafe'babe'b00b'fea2'dad0_u128;
    constexpr uint128_t test_dec = 256'368'684'942'083'501'355'085'096'987'188'714'192_u128;

    static_assert(test_hex == test_dec, "Values are not the same!");
    cout_saver saver{cout};
    cout << std::hex << "Printing hex: [0x" << test_hex << "]." << newl;
    cout << std::dec << "Printing dec: [" << test_hex << "]." << newl;

    using namespace std;
    using namespace cjm::string;
    using cjm::testing::cjm_assert;
    {
        auto default_strm = make_throwing_sstream<char>();
        auto wide_strm = make_throwing_sstream<wchar_t>();
        auto default_hex_strm = make_throwing_sstream<char>();
        auto wide_hex_strm = make_throwing_sstream<wchar_t>();
		auto utf8_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
        auto utf8_hex_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
		auto utf16_stream = make_throwing_sstream<char16_t>();
        auto utf16_hex_stream = make_throwing_sstream<char16_t>();
		auto utf32_stream = make_throwing_sstream<char32_t>();
        auto utf32_hex_stream = make_throwing_sstream<char32_t>();

        default_strm << test_dec;
        wide_strm << test_dec;
        utf8_stream << test_dec;
        utf16_stream << test_dec;
        utf32_stream << test_dec;

        default_hex_strm << test_hex;
        wide_hex_strm << test_hex;
        utf8_hex_stream << test_hex;
        utf16_hex_stream << test_hex;
        utf32_hex_stream << test_hex;
    	

    	 	    	
        uint128_t  v, w, x, y, z,   a, b, c, d, e;

        default_strm >> v;
        wide_strm >> w;
        utf8_stream >> x;
        utf16_stream >> y;
        utf32_stream >> z;

        default_hex_strm >> a;
        wide_hex_strm >> b;
        utf8_hex_stream >> c;
        utf16_hex_stream >> d;
        utf32_hex_stream >> e;
        
        
        cjm_assert(v == test_dec);
        cjm_assert(w == test_dec);
        cjm_assert(x == test_dec);
        cjm_assert(y == test_dec);
        cjm_assert(z == test_dec);

        cjm_assert(a == test_hex);
        cjm_assert(b == test_hex);
        cjm_assert(c == test_hex);
        cjm_assert(d == test_hex);
        cjm_assert(e == test_hex);

        cjm_assert(test_dec == test_hex);

    }

    {
        constexpr std::u8string_view u8as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u8string_view u8as_dec = u8"256368684942083501355085096987188714192"sv;
        constexpr std::u16string_view u16as_hex = u"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u16string_view u16as_dec = u"256368684942083501355085096987188714192"sv;
        constexpr std::u32string_view u32as_hex = U"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u32string_view u32as_dec = U"256368684942083501355085096987188714192"sv;

        uint128_t x = test_hex;
            //uint128_t ::make_from_string(u8as_hex);
        uint128_t y = uint128_t ::make_from_string(u8as_dec);
        cjm_assert(x == y && y == test_hex);
        y = uint128_t::make_from_string(u8as_hex);
        cjm_assert(y == x);

    	//todo fixit add hex parse routines for string conversion ... should not be hard ... the literals already do this.
        x = uint128_t::make_from_string(u16as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);
        x = uint128_t::make_from_string(u16as_hex);
        cjm_assert(x == y && y == test_hex && x == test_dec);
    	
        x = uint128_t::make_from_string(u32as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);
        x = uint128_t::make_from_string(u32as_hex);
        cjm_assert(x == y && y == test_hex && x == test_dec);
    }



}

void cjm::uint128_tests::execute_basic_multiplication_test()
{
    using cjm::testing::cjm_assert;
    using namespace numerics::uint128_literals;

    auto print_res = [](uint128_t res, std::string_view n) -> void
    {
        auto saver = cout_saver{ cout };
        std::cout << n << " hex: [0x" << std::hex << std::setw(32) << std::setfill('0') << res << "]." << newl;
        std::cout << n << " dec: [" << std::dec << res << "]." << newl;
    };

	constexpr uint128_t zero = 0;
    constexpr uint128_t one = 1;
    constexpr uint128_t max64 = std::numeric_limits<std::uint64_t>::max();
    constexpr uint128_t max64_less_one = max64 - 1;
    constexpr uint128_t fit_64 = 0xdead'beef'b00b'face;
    constexpr uint128_t big_one = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
    constexpr uint128_t another_big_one = 0xdead'beef'600d'f00d'badd'd00d'c0de'face_u128;

    constexpr auto zero_times_one = zero * one;
    constexpr auto one_times_one = one * one;
    constexpr auto max64_less_one_times_one = max64_less_one * one;
    static_assert(zero_times_one == 0);
    static_assert(one_times_one == 1);
    static_assert(max64_less_one_times_one == max64_less_one);
	
    constexpr auto max64_times_max64_less_one = max64 * max64_less_one; 
    constexpr auto fit_times_big_one = fit_64 * big_one;
    constexpr auto fit_times_another_big_one = fit_64 * another_big_one;
    constexpr auto big_ones_product = big_one * another_big_one; 
	
    auto test_zero_times_one = zero * one;
    auto test_one_times_one = one * one;
    auto test_max64_less_one_times_one = max64_less_one * one;
    auto test_max64_times_max64_less_one = max64 * max64_less_one;
    auto test_fit_times_big_one = fit_64 * big_one;
    auto test_fit_times_another_big_one = fit_64 * another_big_one;
    auto test_big_ones_product = big_one * another_big_one;

    cjm_assert(test_zero_times_one == zero_times_one);
    cjm_assert(test_one_times_one == one_times_one);
    cjm_assert(test_max64_less_one_times_one == max64_less_one_times_one);
    cjm_assert(test_max64_times_max64_less_one == max64_times_max64_less_one);
    cjm_assert(test_fit_times_big_one == fit_times_big_one);
    cjm_assert(test_fit_times_another_big_one == fit_times_another_big_one);
    cjm_assert(test_big_ones_product == big_ones_product);

    cjm_assert(test_zero_times_one / one == zero);
    cjm_assert(test_one_times_one / one == one);
    cjm_assert(test_max64_times_max64_less_one / max64_less_one == max64 && test_max64_times_max64_less_one / max64 == max64_less_one);


    ctrl_uint128_t ctrl_fit_times_big_one = to_ctrl(test_fit_times_big_one);
    test_interconversion(ctrl_fit_times_big_one, test_fit_times_big_one);
    ctrl_uint128_t ctrl_big_one = to_ctrl(big_one);
    test_interconversion(ctrl_big_one, big_one);
    ctrl_uint128_t ctrl_fit_64 = to_ctrl(fit_64);
    test_interconversion(ctrl_fit_64, fit_64);

    {
        auto save_me = cout_saver{ cout };
        ctrl_uint128_t ctrl_fit_big_div_res = ctrl_fit_times_big_one / ctrl_big_one;

        cjm_assert(to_test(ctrl_fit_times_big_one) == fit_times_big_one && to_test(ctrl_big_one) == big_one);
    	
        cout    << "ctrl_fit_big_div_res:= [0x" << std::hex << std::setw(32)
    			<< std::setfill('0') << ctrl_fit_big_div_res << "]" << newl;
    	
        uint128_t back_test_div_big = to_test(ctrl_fit_big_div_res);
    	cout    << "back_test_div_big := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_big << "]" << newl;

        uint128_t back_test_div_res = fit_times_big_one / big_one;
    	cout    << "back_test_div_res := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_res << "]" << newl;
    	
    	cjm_assert(back_test_div_big == fit_times_big_one / big_one);
    }

	ctrl_uint128_t ctrl_fit_fit_div_res = ctrl_fit_times_big_one / ctrl_fit_64;
    uint128_t back_test_div_fit = to_test(ctrl_fit_fit_div_res);
    cjm_assert(back_test_div_fit == fit_times_big_one / fit_64);
		
    print_res(test_zero_times_one, "test_zero_times_one"sv);
    print_res(test_one_times_one, "test_one_times_one"sv);
    print_res(test_max64_less_one_times_one, "test_max64_less_one_times_one"sv);
    print_res(test_max64_times_max64_less_one, "test_max64_times_max64_less_one"sv);
    print_res(test_fit_times_big_one, "test_fit_times_big_one"sv);
    print_res(test_fit_times_another_big_one, "test_fit_times_another_big_one"sv);
    print_res(test_big_ones_product, "test_big_ones_product"sv);
	
}

void cjm::uint128_tests::test_fls()
{
    constexpr auto final_test = 0x8000'0000'0000'0000u;
    constexpr auto final_res = cjm::numerics::internal::fls_default(final_test);
    constexpr auto numbers = get_pow2_arr();
    constexpr auto results = get_pow2_res_arr();
    static_assert(numbers.size() == results.size());
    static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<uint128_t>);
    using namespace cjm::numerics::uint128_literals;
    static_assert(cjm::numerics::most_sign_set_bit(3_u128) == 1);
    static_assert(cjm::numerics::most_sign_set_bit(3_u128 << 120) == 121);
    auto saver = cout_saver{ cout };
	for (auto i = 0_szt; i < numbers.size(); ++i)
	{
        cjm_assert(cjm::numerics::internal::fls_int_part(numbers[i]) == results[i]);
        cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << numbers[i] << "], the result is: [" << std::dec << results[i] << "]." << newl;
	}
    cjm_assert(cjm::numerics::internal::fls_int_part(final_test) == final_res);
    cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << final_test << "], the result is: [" << std::dec << final_res << "]." << newl;
}

void cjm::uint128_tests::test_interconversion(const ctrl_uint128_t& control, uint128_t test)
{
	const uint128_t ctrl_to_test = to_test(control);
	const ctrl_uint128_t test_to_control = to_ctrl(test);
    cjm::testing::cjm_assert(test_to_control == control && ctrl_to_test == test);
}


void cjm::uint128_tests::print_uint128_eval_mode()
{
    using namespace numerics;
    cout_saver o_saver{cout};
    std::string_view eval_text = "Unknown/Unrecognized Arithmetic Mode"sv;
    switch (cjm::numerics::calculation_mode)
    {
        case uint128_calc_mode::intrinsic_u128:
            eval_text = "Built-in unsigned __int128 Arithmetic"sv;
            break;
        case uint128_calc_mode::msvc_x64:
            eval_text = "Microsoft x64 Compiler-Intrinsic Optimized Arithmetic"sv;
            break;
        case uint128_calc_mode::default_eval:
            eval_text = "Fallback Arithmetic"sv;
            break;
    }
    cout << "Arithmetic mode: [" << eval_text << "]." << newl;
}
void cjm::uint128_tests::print_cpp20_bitops_available()
{
    constexpr auto avail_text = bool_to_yes_no(cjm::numerics::has_cpp20_bitops);
    using namespace numerics;
    cout_saver o_saver{cout};
    cout << "Are C++20 bit operations available?: ["sv << avail_text << "]."sv << newl;
}
void cjm::uint128_tests::print_constexpr_bitcast_available()
{
    constexpr auto avail_text = bool_to_yes_no(cjm::numerics::constexpr_bit_casting);
    using namespace numerics;
    cout_saver o_saver{cout};
    cout << "Is constexpr bitcasting available?: [" << avail_text << "]." << newl;
}

void cjm::uint128_tests::print_builtin_uint128_data_if_present()
{
    if constexpr (cjm::numerics::calculation_mode == numerics::uint128_calc_mode::intrinsic_u128)
    {
        constexpr auto arithmetic_text = bool_to_yes_no(std::is_arithmetic_v<cjm::numerics::natuint128_t>);
        constexpr auto integral_text = bool_to_yes_no(std::is_integral_v<cjm::numerics::natuint128_t>);
        constexpr auto unsigned_text = bool_to_yes_no(std::is_unsigned_v<cjm::numerics::natuint128_t>);

        cout << "Is unsigned __int128 an arithmetic type?: [" << arithmetic_text << "]." << newl;
        cout << "Is unsigned __int128 an integral type?: [" << integral_text << "]." << newl;
        cout << "Is unsigned __int128 an unsigned type?: [" << unsigned_text << "]." << newl;
        cout << "sizeof(std::uintmax_t): [" << sizeof(std::uintmax_t) << "]." << newl;
    }
    else
    {
        cout << "No built-in  uint128 data available." << newl;
    }
}
void cjm::uint128_tests::execute_first_bin_op_test()
{
    using bin_op_t = binary_operation<uint128_t, uint128_ctrl>;
    constexpr auto hash_width = std::numeric_limits<size_t>::digits / 4;
	//perform statically as well:
    static_assert(0x1_u128 + 0x2_u128 == 0x3_u128);
    auto saver = cout_saver {std::cout};
    auto op = bin_op_t { binary_op::add, 1_u128, 2_u128 };
    op.calculate_result();
    cjm_assert(op.has_correct_result());
    const auto& res = op.result();
    cjm_assert(res.value().first == res.value().second);
    std::cout << "Here is the first binary operation:" << newl;
    append_static_assertion(std::cout, op);
    std::cout << "Done appending the assertion." << newl;
    size_t hash = std::hash<binary_operation<uint128_t, uint128_ctrl>>{}(op);
    std::cout   << "hash code for op is: [0x" << std::hex
                << std::setw(hash_width) << std::setfill('0')
                << hash << "]." << newl;
    std::cout << "serialized operation: [" << op << "]" << newl;
    auto ss = string::make_throwing_sstream<char>();
    ss << op;
    bin_op_t round_tripped;
    ss >> round_tripped;
    cjm_assert(round_tripped == op);
}

void cjm::uint128_tests::execute_gen_comp_ops_test()
{
    constexpr ctrl_uint128_t cmax = std::numeric_limits<ctrl_uint128_t>::max();
    constexpr uint128_t  tmax = std::numeric_limits<uint128_t>::max();

    cjm_assert(to_test(cmax) == tmax && to_ctrl(tmax) == cmax && u128_testing_constant_providers::testing_constant_provider<uint128_t>::maximum == tmax);

    auto gen = generator::rgen{};
    auto factory = [](const generator::rgen& g) -> binary_operation<uint128_t, ctrl_uint128_t>
    {
        return create_compare_op(g);
    };

    auto code_to_strong_ordering = [](const uint128_t& c) -> std::strong_ordering
    {
        if (c == 0)
            return std::strong_ordering::equal;
        if (c == 1)
            return std::strong_ordering::greater;
    	return std::strong_ordering::less;
    };

    auto validate_according_to_ordering = [](const uint128_t& l, const uint128_t& r, std::strong_ordering ordering) -> bool
    {
        bool ret;
    	if (ordering == std::strong_ordering::equivalent || ordering == std::strong_ordering::equal)
            ret= l == r && !(l > r) && !(l < r) && (l >= r) && (l <= r) && !(l != r) && (std::hash<uint128_t>{}(l) == std::hash<uint128_t>{}(r));
        else if (ordering == std::strong_ordering::less)

            ret= l != r && (l < r) && !(l > r) && !(l >= r) && (l <= r) && !(l == r);
        else // greater
            ret = l != r && (l > r) && !(l < r) && !(l <= r) && (l >= r) && !(l == r);
        return ret;
    };
	constexpr auto ops = 10'000u;
    auto vec = generator::create_many_ops(factory, gen, ops);
    cjm_assert(vec.size() == ops);
    cjm_assert(std::all_of(vec.cbegin(), vec.cend(), [](const binary_operation<uint128_t, ctrl_uint128_t>& op) -> bool
        {
            return op.op_code() == binary_op::compare;
        }));

	
    cjm_assert(std::all_of(vec.begin(), vec.end(), [=](binary_operation<uint128_t, ctrl_uint128_t>& op) -> bool
    {
            op.calculate_result();
            bool is_ok = false;
            bool thinks_ok = op.has_correct_result();
    		if (thinks_ok)
    		{
                const auto& res = op.result().value();
                std::strong_ordering t_ordering = code_to_strong_ordering(res.first);
                std::strong_ordering c_ordering = code_to_strong_ordering(res.second);
    			if (t_ordering == c_ordering)
    			{
                    is_ok = validate_according_to_ordering(op.left_operand(), op.right_operand(), t_ordering);
    			}    			
    		}
            return is_ok;
    }));
}

namespace cjm::uint128_tests::generator
{
    using twister_t = std::conditional_t<sizeof(std::uintptr_t) == 8, std::mt19937_64, std::mt19937>;
    using twister_seed_t = std::conditional_t<sizeof(std::uintptr_t) == 8, typename std::mt19937_64::result_type, typename std::mt19937::result_type>;

    [[nodiscard]] uint128_t unsafe_create_in_range(const rgen& rgen, size_t size);

    [[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
        unsafe_create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

    [[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
        unsafe_create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);
}
namespace cjm::uint128_tests::generator::internal
{
    class rgen_impl final
    {
    public:
        static std::unique_ptr<rgen_impl> make_rgen();
        ~rgen_impl() = default;
        rgen_impl(const rgen_impl& other) = delete;
        rgen_impl(rgen_impl&& other) noexcept = delete;
        rgen_impl& operator=(const rgen_impl& other) = delete;
        rgen_impl& operator=(rgen_impl&& other) noexcept = delete;

        std::vector<std::uint8_t> generate(size_t bytes);
        int generate_shift_param(int num_digits);
        template<typename Int> requires(std::integral<Int> &&
            !cjm::numerics::concepts::character<Int> && (!std::is_same_v<std::remove_const_t<Int>, std::uint8_t>))
            Int random_from_range(std::uniform_int_distribution<Int> distrib)
        {
            return distrib(m_twister);
        }

    private:
        static twister_t init_twister();
        rgen_impl();
        twister_t m_twister;

    };


}

namespace
{
    using ssize_t = std::make_signed_t<size_t>;

}

std::unique_ptr<cjm::uint128_tests::generator::internal::rgen_impl> cjm::uint128_tests::generator::internal::rgen_impl::make_rgen()
{
    return std::unique_ptr<rgen_impl>(new rgen_impl());
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::internal::rgen_impl::generate(size_t bytes)
{

    auto distrib = std::uniform_int_distribution<std::uint64_t>();
    auto v = std::vector<uint8_t>{};
    v.reserve(bytes);
    auto diff = static_cast<ssize_t>(bytes - v.size());
    if (diff < 0) throw std::invalid_argument{ "Too many bytes ... conversion to signed causes overflow!" };
    do
    {
        auto generated = distrib(m_twister);
        const auto* byte_ptr_begin = reinterpret_cast<unsigned char*>(&generated);
        const auto* byte_ptr_end = byte_ptr_begin + (diff < static_cast<ssize_t>(sizeof(generated)) ? diff : sizeof(generated));
        for (const auto* it = byte_ptr_begin; it != byte_ptr_end; ++it)
        {
            v.push_back(*it);
        }
        diff = static_cast<ssize_t>(bytes - v.size());
    } while (diff > 0);
    return v;
}

int cjm::uint128_tests::generator::internal::rgen_impl::generate_shift_param(int num_digits)
{
    auto distrib = std::uniform_int_distribution<int>(0, num_digits - 1);
    return distrib(m_twister);
}



cjm::uint128_tests::generator::twister_t cjm::uint128_tests::generator::internal::rgen_impl::init_twister()
{
    auto seed_gen = std::random_device{};
    auto seed = seed_gen();
    auto twister = twister_t{ seed };
    return twister;
}

cjm::uint128_tests::generator::internal::rgen_impl::rgen_impl() : m_twister{ init_twister() }
{



}


cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_modulus_op(const rgen& rgen,
        size_t max_dividend_size, size_t max_divisor_size)
{
    if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
        throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
    if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
        throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
    return unsafe_create_modulus_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_modulus_ops(const rgen& rgen,
        size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
    if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
        throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
    if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
        throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
    auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
    if (num_ops > 0)
    {
        vector.reserve(num_ops);
        while (vector.size() < num_ops)
        {
            vector.emplace_back(unsafe_create_modulus_op(rgen, max_dividend_size, max_divisor_size));
        }
    }
    return vector;
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_compare_op(const rgen& rgen)
{
    const auto op = binary_op::compare;
    const uint128_t left = create_random_in_range<uint128_t>(rgen);
    const uint128_t right = create_random_in_range<uint128_t>(rgen);
    return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_shift_op(const rgen& rgen)
{
    const auto op = rgen.random_op(binary_op_category::bitshift);
    assert(op >= first_shift_op && op <= last_shift_op);
    const uint128_t left = create_random_in_range<uint128_t>(rgen);
    const uint128_t right = create_shift_param_for_type<uint128_t>(rgen);
    assert(right >= 0 && right < std::numeric_limits<uint128_t>::digits);
    return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_bw_op(const rgen& rgen)
{
    const auto op = rgen.random_op(binary_op_category::bitwise);
    assert(op >= first_bw_op && op <= last_bw_op);
    const uint128_t left = create_random_in_range<uint128_t>(rgen);
    const uint128_t right = create_random_in_range<uint128_t>(rgen);
    return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_division_op(const rgen& rgen,
        size_t max_dividend_size, size_t max_divisor_size)
{
    if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
        throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
    if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
        throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
    return unsafe_create_division_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
    cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_division_ops(const rgen& rgen,
        size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
    if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
        throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
    if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
        throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
    auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
    if (num_ops > 0)
    {
        vector.reserve(num_ops);
        while (vector.size() < num_ops)
        {
            vector.emplace_back(unsafe_create_division_op(rgen, max_dividend_size, max_divisor_size));
        }
    }
    return vector;
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::create_random_in_range(const rgen& rgen,
    size_t byte_limit)
{
    if (byte_limit == 0) throw std::invalid_argument{ "zero is not a valid size." };
    return unsafe_create_in_range(rgen, byte_limit);
}

cjm::uint128_tests::generator::rgen::rgen() : m_gen()
{
    m_gen = internal::rgen_impl::make_rgen();
    assert(m_gen);
}

cjm::uint128_tests::generator::rgen::rgen(rgen&& other) noexcept : rgen(true)
{
    std::swap(m_gen, other.m_gen);
}

cjm::uint128_tests::generator::rgen& cjm::uint128_tests::generator::rgen::operator=(rgen&& other) noexcept
{
    if (other != *this)
    {
        std::swap(other.m_gen, m_gen);
    }
    return *this;
}

cjm::uint128_tests::binary_op cjm::uint128_tests::generator::rgen::
random_op(binary_op_category category) const
{
    if (category == binary_op_category::compare) //only one in cat.
    {
        return binary_op::compare;
    }

    std::uniform_int_distribution<unsigned> distrib;

    auto to_unsigned = [](binary_op op) -> unsigned { return static_cast<unsigned>(op); };

    switch (category)  // NOLINT(clang-diagnostic-switch-enum)
    {
    default:
    case binary_op_category::any:
        distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_op), to_unsigned(last_op) };
        break;
    case binary_op_category::bitwise:
        distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_bw_op), to_unsigned(last_bw_op) };
        break;
    case binary_op_category::bitshift:
        distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_shift_op), to_unsigned(last_shift_op) };
        break;
    case binary_op_category::add_subtract_multiply:
        distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_add_sub_mul_op), to_unsigned(last_add_sub_mul_op) };
        break;
    case binary_op_category::div_mod:
        distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_divmod_op), to_unsigned(last_divmod_op) };
        break;
    }
    assert(distrib.min() <= distrib.max());

    unsigned value = m_gen->random_from_range(distrib);
    auto ret = static_cast<binary_op>(value);
    assert(ret >= first_op && ret <= last_op);
    return ret;
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::rgen::generate(size_t bytes) const
{
    assert(good());
    return m_gen->generate(bytes);
}
[[nodiscard]] int cjm::uint128_tests::generator::rgen::generate_shift_param(int binary_digits) const
{
    if (binary_digits - 1 <= 0) throw std::invalid_argument{ "binary digits must be at least two." };
    return m_gen->generate_shift_param(binary_digits);
}
cjm::uint128_tests::generator::rgen::rgen(bool) noexcept
{
    m_gen = nullptr;
}

bool cjm::uint128_tests::generator::operator==(const rgen& lhs, const rgen& rhs) noexcept
{
    return lhs.m_gen.get() == rhs.m_gen.get();
}

bool cjm::uint128_tests::generator::operator!=(const rgen& lhs, const rgen& rhs) noexcept
{
    return !(lhs == rhs);
}

size_t std::hash<cjm::uint128_tests::generator::rgen>::operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept
{
    cjm::uint128_tests::generator::internal::rgen_impl* ptr = key.m_gen.get();
    return std::hash<cjm::uint128_tests::generator::internal::rgen_impl*>{}(ptr);
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_division_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
    assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
    const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
    uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
    while (right_operand == 0_u128)
    {
        right_operand = unsafe_create_in_range(rgen, max_divisor_size);
    }
    const auto op = binary_op::divide;
    return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_modulus_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
    assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
    const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
    uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
    while (right_operand == 0_u128)
    {
        right_operand = unsafe_create_in_range(rgen, max_divisor_size);
    }
    const auto op = binary_op::modulus;
    return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::unsafe_create_in_range(const rgen& rgen, size_t size)
{
    using byte_array = typename uint128_t::byte_array;
    assert(rgen.good());
    assert(size > 0 && size <= sizeof(uint128_t));
    auto vec = rgen.generate(size);
    auto arr = byte_array{};
    assert(vec.size() <= arr.size());
    std::copy(vec.cbegin(), vec.cend(), arr.begin());
    return uint128_t::make_from_bytes_little_endian(arr);
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::sv_t text)
{
    auto trimmed = cjm::string::trim_as_sv(text);
    if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
    for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
    {
        if (op_symbol_lookup[idx] == text)
        {
            return static_cast<binary_op>(idx);
        }
    }
    throw std::invalid_argument{"Supplied text is not a binary operation symbol."};

}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::wsv_t text)
{
    auto trimmed = cjm::string::trim_as_sv(text);
    if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
    for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
    {
        if (op_symbol_lookup_wide[idx] == text)
        {
            return static_cast<binary_op>(idx);
        }
    }
    throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u8sv_t text)
{
    auto trimmed = cjm::string::trim_as_sv(text);
    if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
    for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
    {
        if (op_symbol_lookup_u8[idx] == text)
        {
            return static_cast<binary_op>(idx);
        }
    }
    throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u16sv_t text)
{
    auto trimmed = cjm::string::trim_as_sv(text);
    if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
    for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
    {
        if (op_symbol_lookup_u16[idx] == text)
        {
            return static_cast<binary_op>(idx);
        }
    }
    throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u32sv_t text)
{
    auto trimmed = cjm::string::trim_as_sv(text);
    if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
    for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
    {
        if (op_symbol_lookup_u32[idx] == text)
        {
            return static_cast<binary_op>(idx);
        }
    }
    throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

void cjm::uint128_tests::execute_trim_tests()
{

    using namespace std::string_literals;
    using namespace std::string_view_literals;
    constexpr std::string_view should_be = "<<"sv;
    std::string x = " <<   "s;
    std::string_view xv = x;
    std::string x_trim = cjm::string::trim(x);
    cjm_deny(x == x_trim);
    cjm_assert(x_trim == should_be);
    x = " <<   "s;
    xv = x;
    xv = cjm::string::trim_as_sv(xv);
    cjm_assert(xv == should_be);
    cjm_deny(x == should_be);
}

void cjm::uint128_tests::execute_stream_insert_bin_op_test()
{
    auto default_init_wrong_val = [](binary_op o) -> binary_op
    {
        constexpr unsigned biggest = static_cast<unsigned>(last_op);
        constexpr unsigned modulo = biggest + 1;
        auto unsigned_val = (static_cast<unsigned>(o) + 1) % modulo;
        auto ret = static_cast<binary_op>(unsigned_val);
        assert(ret >= first_op && ret <= last_op && ret != o);
        return ret;
    };
    for (auto op_int = static_cast<unsigned>(cjm::uint128_tests::first_op);
        op_int <= static_cast<unsigned>(cjm::uint128_tests::last_op); ++op_int )
    {
        auto op = static_cast<binary_op>(op_int);
        const auto wrong_op = default_init_wrong_val(op);
        auto nstream = cjm::string::make_throwing_sstream<char>();
        auto wstream = cjm::string::make_throwing_sstream<wchar_t>();
        auto u8stream = cjm::string::make_throwing_sstream<char8_t>();
        auto u16stream = cjm::string::make_throwing_sstream<char16_t>();
        auto u32stream = cjm::string::make_throwing_sstream<char32_t>();

        nstream << op;
        wstream << op;
        u8stream << op;
        u16stream << op;
        u32stream << op;

        binary_op rt_op_n;
        nstream >> rt_op_n;
        cjm_assert(rt_op_n == op);

        binary_op rt_op_w;
        wstream >> rt_op_w;
        cjm_assert(rt_op_w == op);

        binary_op rt_op_u8 = wrong_op;
        cjm_assert(rt_op_u8 != op);
        u8stream >> rt_op_u8;
        cjm_assert(rt_op_u8 == op);

        binary_op rt_op_u16 = wrong_op;
        u16stream >> rt_op_u16;
        cjm_assert(rt_op_u16 == op);

        binary_op rt_op_u32 = wrong_op;
        u32stream >> rt_op_u32;
        cjm_assert(rt_op_u32 == op);
    }
}

//void cjm::uint128_tests::execute_gen_comp_ops_test()
//{
//    auto rgen = cjm::uint128_tests::generator::rgen{};
//    const size_t num_ops = 10;
//	
//    auto v = generator::create_compare_op(rgen);
//    cjm_assert(v.size() == num_ops);
//}

#ifdef CJM_HAVE_BUILTIN_128
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
    static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<unsigned __int128>);
    unsigned __int128 x = 3;
    auto most_sign_bit = cjm::numerics::most_sign_set_bit(x);
    std::cout << "Test for 3 yields bitpos: [" << most_sign_bit << "]." << newl;
    cjm_assert(most_sign_bit == 1);
    x <<= 120;
    most_sign_bit = cjm::numerics::most_sign_set_bit(x);
    std::cout << "Test for (3 << 120) yields: [" << most_sign_bit << "]." << newl;
    cjm_assert(most_sign_bit == 121);

}
#else
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
    std::cout << "Will not test builtin_u128_fls because not available in this enviornment." << newl;
}


#endif



