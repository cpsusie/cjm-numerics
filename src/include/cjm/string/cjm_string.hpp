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
//
// This library is modeled on the abseil C++ numerics library.  MOST OF ABSEIL'S CODE HAS BEEN MODIFIED SIGNIFICANTLY BY THIS LIBRARY.
// Some verbatim reproduction may remain in places.  The following notice was found in abseil's uint128 source code and is reproduced here in
// accordance with the terms of the Apache 2.0 license, under which Abseil's code was licensed to CJM Screws, LLC:
#ifndef CJM_STRING_HPP_
#define CJM_STRING_HPP_
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>
#include <cjm/numerics/cjm_configuration.hpp>
#include <cjm/numerics/numerics_configuration.hpp>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <cjm/string/istream_utils.hpp>
namespace cjm::string
{

	
	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
	std::basic_string<Char, CharTraits, Allocator> trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe);

	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
		requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
	std::basic_string<Char, CharTraits, Allocator> trim(std::basic_string<Char, CharTraits, Allocator>&& trim_me);
	
	template<typename Char, typename CharTraits>
		requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_string_view<Char, CharTraits> trim_as_sv(std::basic_string_view<Char, CharTraits> trimMe);

	
	using u8_ofstream_t = std::basic_ofstream<char8_t, std::char_traits<char8_t>>;
	using u16_ofstream_t = std::basic_ofstream<char16_t, std::char_traits<char16_t>>;
	using u32_ofstream_t = std::basic_ofstream<char32_t, std::char_traits<char32_t>>;

	using u8_ifstream_t = std::basic_ifstream<char8_t, std::char_traits<char8_t>>;
	using u16_ifstream_t = std::basic_ifstream<char16_t, std::char_traits<char16_t>>;
	using u32_ifstream_t = std::basic_ifstream<char32_t, std::char_traits<char32_t>>;

	template<numerics::concepts::utf8_char_allocator Allocator = std::allocator<char8_t>>
	using u8string_stream_t = std::basic_stringstream<char8_t, std::char_traits<char8_t>, Allocator>;
	template<numerics::concepts::utf16_char_allocator Allocator = std::allocator<char16_t>>
	using u16string_stream_t = std::basic_stringstream<char16_t, std::char_traits<char16_t>, Allocator>;
	template<numerics::concepts::utf32_char_allocator Allocator = std::allocator<char32_t>>
	using u32string_stream_t = std::basic_stringstream<char32_t, std::char_traits<char32_t>, Allocator>;

	using path_t = std::filesystem::path;
	using path_char_t = std::filesystem::path::value_type;
	using path_str_t = std::filesystem::path::string_type;
	using path_sv_t = std::basic_string_view<path_char_t>;
	using path_format_t = std::filesystem::path::format;


	template<numerics::concepts::basic_string_stream TStringStream>
	auto move_extract_string(TStringStream&& stream)
		-> std::basic_string<typename TStringStream::char_type, typename TStringStream::traits_type, typename TStringStream::allocator_type>
	{
		return std::move((*(stream.rdbuf())).str());
	}
	
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

	template<typename Char, typename CharTraits = std::char_traits<Char>>
		requires numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_ifstream<Char, CharTraits> make_throwing_ifstream(std::string_view file_name)
	{
		using stream_t = std::basic_ifstream<Char, CharTraits>;
		auto stream = stream_t{ file_name.data() };
		stream.exceptions(stream_t::failbit | stream_t::badbit);
		return stream;
	}

	template<typename Char, typename CharTraits = std::char_traits<Char>>
		requires numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_ofstream<Char, CharTraits> make_throwing_ofstream(path_t file_name)
	{
		using stream_t = std::basic_ofstream<Char, CharTraits>;
		auto stream = stream_t{ file_name };
		stream.exceptions(stream_t::failbit | stream_t::badbit);
		return stream;
	}

	template<typename Char, typename CharTraits = std::char_traits<Char>>
		requires numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_ifstream<Char, CharTraits> make_throwing_ifstream(path_t file_name)
	{
		using stream_t = std::basic_ifstream<Char, CharTraits>;
		auto stream = stream_t{ file_name };
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
		friend std::basic_string_view<Char, CharTraits> trim_as_sv(std::basic_string_view<Char, CharTraits> trim_me);
		template<typename Char, typename CharTraits, typename Allocator>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
		friend std::basic_string<Char, CharTraits, Allocator> trim(std::basic_string<Char, CharTraits, Allocator>&& trim_me);
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

	template<typename Char, typename Traits = std::char_traits<Char>>
		requires (cjm::numerics::concepts::char_or_wchar_t_with_traits<Char, Traits>)
	struct case_ignoring_trimmed_ordinal_compare
	{
		std::weak_ordering compare(std::basic_string_view<Char, Traits> lhs, std::basic_string_view<Char, Traits> rhs) const noexcept
		{
			using char_t = typename std::basic_string_view<Char, Traits>::value_type;
			auto l_trimmed = trim_as_sv(lhs);
			auto r_trimmed = trim_as_sv(rhs);

			if (l_trimmed.empty())
			{
				return r_trimmed.empty() ? std::weak_ordering::equivalent : std::weak_ordering::less;
			}
			if (r_trimmed.empty())
			{
				return l_trimmed.empty() ? std::weak_ordering::equivalent : std::weak_ordering::greater;
			}

			const size_t shorter = std::min(l_trimmed.size(), r_trimmed.size());
			const auto locale = std::locale("");
			for (size_t i = 0; i < shorter; ++i)
			{
				char l_char_temp = std::tolower<char_t>(lhs[i], locale);
				char r_char_temp = std::tolower<char_t>(rhs[i], locale);
				if (auto temp = l_char_temp <=> r_char_temp; temp != std::strong_ordering::equivalent)
				{
					return temp;
				}
			}
			return l_trimmed.size() <=> r_trimmed.size();			
		}
	};

	template<typename Char, typename Traits = std::char_traits<Char>>
		requires (cjm::numerics::concepts::char_or_wchar_t_with_traits<Char, Traits>)
	struct case_ignoring_trimmed_ordinal_less
	{
		bool operator()(std::basic_string_view<Char, Traits> lhs, std::basic_string_view<Char, Traits> rhs) const noexcept
		{
			return s_comparer.compare(lhs, rhs) == std::weak_ordering::less;
		}
		static constexpr auto s_comparer = case_ignoring_trimmed_ordinal_compare<Char, Traits>{};
	};

	
}
#include <cjm/string/cjm_string.inl>
#endif // ! CJM_STRING_HPP_