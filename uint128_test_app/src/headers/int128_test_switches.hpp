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
// ReSharper disable CppClangTidyClangDiagnosticSwitch
// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#ifndef CJM_INT128_TEST_SWITCHES_HPP_
#define CJM_INT128_TEST_SWITCHES_HPP_
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <array>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <cjm/string/cjm_string.hpp>
#include <boost/container_hash/hash.hpp>
#include <functional>
#include <utility>
#include <span>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <cjm/string/istream_utils.hpp>
namespace cjm::uint128_tests::switches
{
	class test_switch;
}

namespace std
{
	template<>
	struct hash<cjm::uint128_tests::switches::test_switch> final
	{
		[[nodiscard]] size_t operator()(const cjm::uint128_tests::switches::test_switch& key) const noexcept;
	};
}

namespace cjm::uint128_tests::switches
{
	using namespace std::string_view_literals;
	using sv_t = std::string_view;
	using str_t = std::string;
	using test_mode_underlying_t = std::uint16_t;
	constexpr std::array<sv_t, 3> switch_tokens = {"--"sv,"-"sv,"/"sv };
	enum class test_mode : test_mode_underlying_t;
	constexpr test_mode operator|(test_mode lhs, test_mode rhs) noexcept;
	constexpr test_mode operator&(test_mode lhs, test_mode rhs) noexcept;
	constexpr test_mode operator^(test_mode lhs, test_mode rhs) noexcept;
	constexpr test_mode operator~(test_mode operand) noexcept;
	constexpr test_mode& operator|=(test_mode& lhs, test_mode rhs) noexcept;
	constexpr test_mode& operator&=(test_mode& lhs, test_mode rhs) noexcept;
	constexpr test_mode& operator^=(test_mode& lhs, test_mode rhs) noexcept;
	constexpr bool has_flag(test_mode query, test_mode flag) noexcept;
	constexpr std::optional<bool> needs_parameter(test_mode individual_flag) noexcept;
	constexpr std::optional<bool> should_not_have_parameter(test_mode individual_flag) noexcept;
	constexpr bool is_unspecified_or_combo_of_known_flags(test_mode mode) noexcept;
	constexpr std::optional<std::string_view> get_text_for_flag_combo(test_mode mode) noexcept;
	constexpr std::optional<std::string_view> get_text_for_indiv_flag(test_mode mode) noexcept;
	std::string get_text_any_mode(test_mode mode);
	std::pair<std::string, std::vector<std::string>> normalize_and_stringify_console_args(int argc, char* argv[]);
	std::vector<test_switch> process_input(std::span<std::string> args);
	

	template<numerics::concepts::character Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os, const test_mode& mode);
	
	enum class test_mode : test_mode_underlying_t
	{
		unspecified = 0x00,
		execute_unary_from_file = 0x01,
		execute_binary_from_file = 0x02,
		print_environ_info = 0x04,
		run_default_tests = 0x08
	};
	using text_mode_t = std::pair<sv_t, test_mode>;

	constexpr bool is_single_specified_flag(test_mode mode) noexcept;
	constexpr size_t num_specified_flags = 4;
	constexpr auto individual_specified_flags = std::array<test_mode, num_specified_flags>
	{
		test_mode::execute_unary_from_file,
		test_mode::execute_binary_from_file,
		test_mode::print_environ_info,
		test_mode::run_default_tests
	};
	constexpr auto text_mode_lookup = std::array<text_mode_t, num_specified_flags>
	{
		std::make_pair("unary_from_file"sv, test_mode::execute_unary_from_file),
		std::make_pair("binary_from_file"sv, test_mode::execute_binary_from_file),
		std::make_pair("environ"sv, test_mode::print_environ_info),
		std::make_pair("default_tests"sv, test_mode::run_default_tests)
	};
	constexpr auto flags_requiring_parameter = std::array<test_mode, 2>{test_mode::execute_unary_from_file,
		test_mode::execute_binary_from_file};
	std::weak_ordering operator<=>(const test_switch& lhs, const test_switch& rhs) noexcept;

	bool starts_with_switch_token(sv_t text) noexcept;

	class test_switch final
	{
	public:
		static constexpr test_mode default_mode() noexcept;
		using path_t = std::filesystem::path;
		[[nodiscard]] test_mode mode() const noexcept;
		[[nodiscard]] size_t hash_code() const noexcept;
		[[nodiscard]] const std::optional<std::filesystem::path>& file_path() const noexcept;

		test_switch();
		test_switch(test_mode mode, std::optional<std::string> parameter);
		test_switch(const test_switch& other);
		test_switch(test_switch&& other) noexcept;
		test_switch& operator=(const test_switch& other);
		test_switch& operator=(test_switch&& other) noexcept;
		~test_switch();

		friend std::weak_ordering operator<=>(const test_switch& lhs, const test_switch& rhs) noexcept;
		friend bool operator==(const test_switch& lhs, const test_switch& rhs) noexcept = default;
		friend bool operator!=(const test_switch& lhs, const test_switch& rhs) noexcept = default;
		friend bool operator>(const test_switch& lhs, const test_switch& rhs) noexcept = default;
		friend bool operator<(const test_switch& lhs, const test_switch& rhs) noexcept = default;
		friend bool operator>=(const test_switch& lhs, const test_switch& rhs) noexcept = default;
		friend bool operator<=(const test_switch & lhs, const test_switch & rhs) noexcept = default;
				
	
	private:
		struct test_switch_impl;
		std::unique_ptr<test_switch_impl> m_impl;
	};

	class unrecognized_switch : public std::runtime_error
	{

	public:
		explicit unrecognized_switch(sv_t unknown_switch_text);
		~unrecognized_switch() override = default;
	private:
		explicit unrecognized_switch(const std::string& arg);
		static std::string create_message(std::string_view unrecognized_switch_text);
	};

	class not_a_switch : public std::runtime_error
	{
	public:
		explicit not_a_switch(sv_t not_a_switch);
		~not_a_switch() override = default;
	private:
		explicit not_a_switch(const std::string& arg);
		static std::string create_message(std::string_view not_switch_text);
	};

	class bad_test_switch : public std::runtime_error
	{
	public:
		
		bad_test_switch(test_mode mode, std::string_view parameter);
		bad_test_switch(test_mode mode, std::string_view message, std::string_view parameter);
		~bad_test_switch() override = default;
	private:
		
		explicit bad_test_switch(const std::string& message)
			: runtime_error(message) {}
		static std::string create_message(test_mode mode, std::string_view parameter);
		static std::string create_message(test_mode mode, std::string_view message, std::string_view parameter);
	};
	
	class missing_parameter : public std::runtime_error
	{
	public:
		explicit missing_parameter(test_mode mode);
		~missing_parameter() override = default;
	private:
		explicit missing_parameter(const std::string& txt);
		static std::string create_message(test_mode mode);
	};

	template<numerics::concepts::character Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& os, const test_mode& mode)
	{
		using char_t = std::remove_const_t<Char>;
		using local_str_t = std::basic_string<char_t>;
		if constexpr (std::is_same_v<char_t, char>)
		{
			local_str_t text = get_text_any_mode(mode);
			os << text;
			return os;
		}
		else
		{
			std::string text = get_text_any_mode(mode);
			local_str_t converted;
			converted.reserve(text.length());
			std::transform(text.cbegin(), text.cend(),
				std::back_inserter(converted), [](char c) -> char_t
				{
					return convert_char<char, char_t>(c);
				});
			os << converted;
			return os;
		}
	}
}

constexpr cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::operator|(test_mode lhs,
	test_mode rhs) noexcept
{
	auto temp = static_cast<test_mode_underlying_t>(static_cast<test_mode_underlying_t>(lhs) 
		| static_cast<test_mode_underlying_t>(rhs));
	return static_cast<test_mode>(temp);
}

constexpr cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::operator&(test_mode lhs, 
	test_mode rhs) noexcept
{
	auto temp = static_cast<test_mode_underlying_t>(static_cast<test_mode_underlying_t>(lhs) 
		& static_cast<test_mode_underlying_t>(rhs));
	return static_cast<test_mode>(temp);
}

constexpr cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::operator^(test_mode lhs, 
	test_mode rhs) noexcept
{
	auto temp = static_cast<test_mode_underlying_t>(static_cast<test_mode_underlying_t>(lhs) 
		^ static_cast<test_mode_underlying_t>(rhs));
	return static_cast<test_mode>(temp);
}

constexpr cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::operator~
	(test_mode operand) noexcept
{
	auto temp = static_cast<test_mode_underlying_t>(~(static_cast<test_mode_underlying_t>(operand)));
	return static_cast<test_mode>(temp);
}

constexpr cjm::uint128_tests::switches::test_mode& cjm::uint128_tests::switches::operator|=(test_mode& lhs,
	test_mode rhs) noexcept
{
	return (lhs = (lhs | rhs));
}
constexpr cjm::uint128_tests::switches::test_mode& cjm::uint128_tests::switches::operator&=(test_mode& lhs,
	test_mode rhs) noexcept
{
	return (lhs = (lhs & rhs));
}

constexpr cjm::uint128_tests::switches::test_mode& cjm::uint128_tests::switches::operator^=(test_mode& lhs,
	test_mode rhs) noexcept
{
	return (lhs = (lhs ^ rhs));
}

constexpr bool  cjm::uint128_tests::switches::is_single_specified_flag(test_mode mode) noexcept
{
	//reconsider switching after confirm gcc and clang and msvc++ all support constexpr anyof / allof
	for (auto itm : individual_specified_flags)	// NOLINT(readability-use-anyofallof) 
	{
		if (mode == itm) return true;
	}
	return false;
}

constexpr cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::test_switch::default_mode() noexcept
{
	return test_mode::print_environ_info | test_mode::run_default_tests;
}



constexpr bool cjm::uint128_tests::switches::has_flag(test_mode query, test_mode flag) noexcept
{
	if (query == test_mode::unspecified) return false;
	if (!is_single_specified_flag(flag)) return false;
	return (query & flag) == flag;	
}

constexpr std::optional<bool> cjm::uint128_tests::switches::needs_parameter(test_mode individual_flag) noexcept
{
	if (!is_single_specified_flag(individual_flag)) return std::nullopt;
	for (auto itm : flags_requiring_parameter)
	{
		if (individual_flag == itm) return true;
	}
	return false;
	
}

constexpr std::optional<bool> cjm::uint128_tests::switches::should_not_have_parameter(
	test_mode individual_flag) noexcept
{
	auto temp = needs_parameter(individual_flag);
	if (temp.has_value())
	{
		return !(*(temp));
	}
	return std::nullopt;
}

constexpr bool cjm::uint128_tests::switches::is_unspecified_or_combo_of_known_flags(test_mode mode) noexcept
{
	if (mode == test_mode::unspecified) return true;

	constexpr auto inverse_mask = ~(test_mode::print_environ_info | test_mode::run_default_tests | test_mode::execute_binary_from_file | test_mode::execute_unary_from_file);
	return  (mode & inverse_mask) == test_mode::unspecified;	
}

#ifdef CJM_DETECTED_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#endif
constexpr std::optional<std::string_view> cjm::uint128_tests::switches::get_text_for_flag_combo(test_mode mode) noexcept
{
	using namespace std::string_view_literals;
	if (!is_unspecified_or_combo_of_known_flags(mode)) return std::nullopt;
	switch (mode)
	{
	case test_mode::unspecified: 
		return "unspecified"sv;		
	case test_mode::execute_unary_from_file:
		return "execute_unary_from_file"sv;
	case test_mode::execute_binary_from_file: 
		return "execute_binary_from_file"sv;
	case test_mode::print_environ_info:
		return "print_environ_info"sv;
	case test_mode::run_default_tests:
			return "run_default_tests"sv;
		// ReSharper disable CppRedundantParentheses
	case (test_mode::execute_unary_from_file | test_mode::execute_binary_from_file | test_mode::print_environ_info | test_mode::run_default_tests):
		return "execute_unary_from_file | execute_binary_from_file | print_environ_info | run_default_tests"sv;
	case (test_mode::execute_unary_from_file | test_mode::execute_binary_from_file | test_mode::print_environ_info ):
		return "execute_unary_from_file | execute_binary_from_file | print_environ_info"sv;
	case (test_mode::execute_unary_from_file | test_mode::execute_binary_from_file |  test_mode::run_default_tests):
		return "execute_unary_from_file | execute_binary_from_file | run_default_tests"sv;
	case (test_mode::execute_unary_from_file | test_mode::print_environ_info | test_mode::run_default_tests):
		return "test_mode::execute_unary_from_file | test_mode::print_environ_info | test_mode::run_default_tests"sv;
	case (test_mode::execute_unary_from_file | test_mode::execute_binary_from_file):
		return "test_mode::execute_unary_from_file | test_mode::execute_binary_from_file"sv;
	case (test_mode::execute_unary_from_file | test_mode::print_environ_info):
		return "test_mode::execute_unary_from_file | test_mode::print_environ_info"sv;
	case (test_mode::execute_unary_from_file | test_mode::run_default_tests):
		return "test_mode::execute_unary_from_file | test_mode::run_default_tests"sv;
	case (test_mode::execute_binary_from_file | test_mode::print_environ_info | test_mode::run_default_tests):
		return "test_mode::execute_binary_from_file | test_mode::print_environ_info | test_mode::run_default_tests"sv;
	case (test_mode::execute_binary_from_file | test_mode::print_environ_info):
		return "test_mode::execute_binary_from_file | test_mode::print_environ_info"sv;
	case (test_mode::execute_binary_from_file | test_mode::run_default_tests):
		return "test_mode::execute_binary_from_file | test_mode::run_default_tests"sv;
	case (test_mode::print_environ_info | test_mode::run_default_tests):
		return "test_mode::print_environ_info | test_mode::run_default_tests"sv;
	default: 
		return std::nullopt;
		// ReSharper restore CppRedundantParentheses
	}
}
#ifdef CJM_DETECTED_GCC
#pragma GCC diagnostic pop
#endif

constexpr std::optional<std::string_view> cjm::uint128_tests::switches::
get_text_for_indiv_flag(test_mode mode) noexcept
{
	using namespace std::string_view_literals;
	if (mode != test_mode::unspecified && !is_single_specified_flag(mode)) return std::nullopt;
	switch (mode)
	{
	case test_mode::unspecified: 
		return "unspecified"sv;
	case test_mode::execute_unary_from_file:
		return "execute_unary_from_file"sv;		
	case test_mode::execute_binary_from_file: 
		return "execute_binary_from_file"sv;	
	case test_mode::print_environ_info: 
		return "print_environ_info"sv;		
	case test_mode::run_default_tests:
		return "run_default_tests"sv;		
	default:  // NOLINT(clang-diagnostic-covered-switch-default) 
		return std::nullopt;//better than undefined behavior....
	}
}


#endif
