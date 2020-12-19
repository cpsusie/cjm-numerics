#include "int128_tests.hpp"


void cjm::uint128_tests::execute_uint128_tests()
{
    execute_test(execute_basic_test_one, "basic_test_one"sv);
    execute_test(execute_string_parse_test, "string_parse_text"sv);
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
//        auto utf8_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
//        auto utf16_stream = make_throwing_sstream<char16_t>();
//        auto utf32_stream = make_throwing_sstream<char32_t>();

        default_strm << test_dec;
        wide_strm << test_dec;

        std::string text = default_strm.str();
        std::wstring wtext = wide_strm.str();

        uint128_t x = uint128_t::make_from_string(std::string_view{text});
        uint128_t y = uint128_t::make_from_string(std::wstring_view{wtext});

        cjm_assert(x == test_hex);
        cjm_assert(y == test_dec);
        cjm_assert(x == y);
    }
#if defined __clang__ || defined _MSC_VER
    {
        constexpr std::u8string_view u8as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0";
        constexpr std::u8string_view u8as_dec = u8"256368684942083501355085096987188714192";
        constexpr std::u8string_view u16as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0";
        constexpr std::u8string_view u16as_dec = u8"256368684942083501355085096987188714192";
        constexpr std::u8string_view u32as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0";
        constexpr std::u8string_view u32as_dec = u8"256368684942083501355085096987188714192";

        uint128_t x = test_hex;
            //uint128_t ::make_from_string(u8as_hex);
        uint128_t y = uint128_t ::make_from_string(u8as_dec);
        cjm_assert(x == y && y == test_hex);

    	//todo fixit add hex parse routines for string conversion ... should not be hard ... the literals already do this.
        x = uint128_t::make_from_string(u16as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);

        x = uint128_t::make_from_string(u32as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);
    }
#endif


}


