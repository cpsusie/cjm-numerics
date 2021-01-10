#ifndef CJM_STRING_HPP_
#define CJM_STRING_HPP_
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>
#include "cjm_numeric_concepts.hpp"
#include "numerics.hpp"
#include "istream_utils.hpp"
namespace cjm::string
{
	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
	        requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
	std::basic_string<Char, CharTraits, Allocator> trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe);
    template<typename Char, typename CharTraits>
		requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_string_view<Char, CharTraits> trim_as_sv(std::basic_string_view<Char, CharTraits> trimMe);

    
    using u8_ofstream_t = std::basic_ofstream<char8_t, std::char_traits<char8_t>>;
    using u16_ofstream_t = std::basic_ofstream<char16_t, std::char_traits<char16_t>>;
    using u32_ofstream_t = std::basic_ofstream<char32_t, std::char_traits<char32_t>>;

    template<numerics::concepts::utf8_char_allocator Allocator = std::allocator<char8_t>>
    using u8string_stream_t = std::basic_stringstream<char8_t, std::char_traits<char8_t>, Allocator>;
    template<numerics::concepts::utf16_char_allocator Allocator = std::allocator<char16_t>>
    using u16string_stream_t = std::basic_stringstream<char16_t, std::char_traits<char16_t>, Allocator>;
    template<numerics::concepts::utf32_char_allocator Allocator = std::allocator<char32_t>>
    using u32string_stream_t = std::basic_stringstream<char32_t, std::char_traits<char32_t>, Allocator>;

    template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    requires numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
    std::basic_stringstream<Char, CharTraits, Allocator> make_throwing_sstream()
    {
        using stream_t = std::basic_stringstream<Char, CharTraits, Allocator>;
        stream_t ret;
        ret.exceptions(stream_t::failbit | stream_t::badbit);
        return ret;
    }

    template<typename Char, typename CharTraits = std::char_traits<Char>>
    requires numerics::concepts::char_with_traits<Char, CharTraits>
        std::basic_ofstream<Char, CharTraits> make_throwing_ofstream(std::string_view file_name)
    {
        using stream_t = std::basic_ofstream<Char, CharTraits>;
        auto stream = stream_t{ file_name.data() };
        stream.exceptions(stream_t::failbit | stream_t::badbit);
        return stream;
    }

	class string_helper
	{
	public:
		template<typename Char, typename CharTraits, typename Allocator>
		    requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
		friend std::basic_string<Char, CharTraits, Allocator> trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe);
		template<typename Char, typename CharTraits>
			requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
		friend std::basic_string_view<Char, CharTraits> trim_as_sv(std::basic_string_view<Char, CharTraits> trimMe);
		string_helper() = delete;
		~string_helper() = delete;
		string_helper(const string_helper& other) = delete;
		string_helper(string_helper&& other) noexcept = delete;
		string_helper& operator=(const string_helper& other) = delete;
		string_helper& operator=(string_helper&& other) noexcept = delete;
	private:
		

        template<typename Char, typename CharTraits = std::char_traits<Char>>
            requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
        static void ltrim(std::basic_string_view<Char, CharTraits>& l_trim_me);
        template<typename Char, typename CharTraits = std::char_traits<Char>>
            requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
        static void rtrim(std::basic_string_view<Char, CharTraits>& r_trim_me);

	};

	
}
#endif // ! CJM_STRING_HPP_
#include "cjm_string.inl"


