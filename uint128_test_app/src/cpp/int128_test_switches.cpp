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
#include "int128_test_switches.hpp"
#include <sstream>
#include <cassert>
#include <boost/algorithm/string.hpp>

namespace cjm::uint128_tests::switches
{
	test_mode match_switch(sv_t text);

	sv_t strip_switch_token(sv_t text);

	using switch_variant_t = std::variant<std::filesystem::path, std::string, std::monostate>;
	
	struct test_switch::test_switch_impl final
	{
		friend class test_switch;

		[[nodiscard]] std::optional<std::filesystem::path> file_path() const noexcept
		{
			if (m_mode != test_mode::run_specific_test)
			{
				if (m_parameter.index() == 0)
					return std::make_optional(std::get<0>(m_parameter));
				return std::nullopt;
			}
			return std::nullopt;
		}

		[[nodiscard]] std::string_view test_name() const noexcept
		{
			std::string_view ret;
			if (m_mode == test_mode::run_specific_test && m_parameter.index() == 1)
			{
				ret = std::get<1>(m_parameter);
			}
			return ret;
		}
		
		[[nodiscard]] test_mode mode() const noexcept { return m_mode; }
		test_switch_impl(std::string param, test_mode mode);
		test_switch_impl();
		test_switch_impl(const test_switch_impl& other) = default;
		test_switch_impl(test_switch_impl&& other) noexcept = default;
		test_switch_impl& operator=(const test_switch_impl& other) = default;
		test_switch_impl& operator=(test_switch_impl&& other) noexcept = default;
		~test_switch_impl() = default;
	private:
		switch_variant_t m_parameter;
		test_mode m_mode;
	};
}

namespace
{
	constexpr auto newl = "\n";
	std::optional<std::filesystem::path> create_path(std::string_view text)
	{
		if (text.empty())
		{
			return std::nullopt;
		}
		try
		{
			return std::filesystem::path(text);
		}
		catch (const std::exception& ex)
		{
			std::cerr	<<	"Specified text: [" << text << "] cannot be made into a path.  "
						<<	"Exception msg: [" << ex.what() << "]." << newl;
			return std::nullopt;
		}
	}

	cjm::uint128_tests::switches::switch_variant_t create_param_member_if_needed(cjm::uint128_tests::switches::test_mode mode, std::string_view text)
	{
		if (mode == cjm::uint128_tests::switches::test_mode::execute_binary_from_file 
			|| mode == cjm::uint128_tests::switches::test_mode::execute_unary_from_file)
		{
			return create_path(text).value_or(std::string{});
		}
		if (mode == cjm::uint128_tests::switches::test_mode::run_specific_test)
		{
			return cjm::uint128_tests::switches::switch_variant_t{ std::string{text} };
		}
		return cjm::uint128_tests::switches::switch_variant_t{ std::monostate{} };
	}
}

cjm::uint128_tests::switches::test_switch::test_switch_impl::test_switch_impl(std::string param, test_mode mode)
	: m_parameter{create_param_member_if_needed(mode, param)}, m_mode{mode}
{
	using namespace std::string_view_literals;
	sv_t view = param;
	if (mode != test_mode::unspecified && !is_single_specified_flag(mode))
	{
		throw bad_test_switch{ mode, "Bad compound mode: mode."sv, view };
	}
	else
	{
		auto needs_param = needs_parameter(mode).value();
		if (needs_param)
		{
			if (m_mode == test_mode::run_specific_test)
			{
				if (auto ptr = std::get_if<std::string>(&m_parameter))
				{
					if (ptr->empty())
					{
						throw bad_test_switch{mode, "Mode requires specification of a test name, but none was supplied." };
					}
				}
				else
				{
					throw bad_test_switch{ mode, "Mode requires specification of a test name, but none was supplied." };
				}
			}
			else if (m_mode == test_mode::execute_binary_from_file || m_mode == test_mode::execute_unary_from_file)
			{
				if (auto ptr = std::get_if<std::filesystem::path>(&m_parameter))
				{
					if (ptr->empty())
					{
						throw bad_test_switch{ mode, "Mode requires specification of a file, but none was supplied." };
					}
					if (!exists(*ptr))
					{
						std::basic_stringstream<char> msg = cjm::string::make_throwing_sstream<char>();
						msg
							<< "Mode requires file parameter referring to existing file.  Referenced file ["
							<< *ptr << "] does not exist.";
						std::string txt = string::move_extract_string<std::stringstream>(std::move(msg));
						std::string_view str_view = txt;
						throw bad_test_switch(mode, str_view, param);
					}
				}
				else
				{
					throw bad_test_switch{ mode, "Mode requires specification of a test name, but none was supplied." };
				}
			}
		}		
	}
}

cjm::uint128_tests::switches::test_switch::test_switch_impl::test_switch_impl()
	: m_parameter{}, m_mode{default_mode()} {}

std::string cjm::uint128_tests::switches::get_text_any_mode(test_mode mode)
{
	if (auto text = get_text_for_indiv_flag(mode); text.has_value())
	{
		return std::string{ *text };
	}
	
	auto str = cjm::string::make_throwing_sstream<char>();
	constexpr size_t digits = std::numeric_limits<test_mode_underlying_t>::digits;
	constexpr size_t width = digits / 4;
	static_assert(width >= 1, "Unexpected width for underlying type of test_mode enum class.");
	str << "Unrecognized test switch has bit pattern: [0x" << std::hex << std::setw(width)
		<< std::setfill('0') << static_cast<test_mode_underlying_t>(mode) << "].";
	return str.str();
}

std::pair<std::string, std::vector<std::string>> cjm::uint128_tests::switches::normalize_and_stringify_console_args(
	int argc, char* argv[])
{
	if (argc < 1) throw std::invalid_argument{ "The first parameter must always be greater than or equal to one." };
	if (!argv) throw std::invalid_argument{ "The second parameter must not be null." };
	std::string current_exec_temp = argv[0];
	current_exec_temp = cjm::string::trim(current_exec_temp);

	std::string current_exec = std::move(current_exec_temp);
	
	std::vector<std::string> ret;
	if (argc > 1)
	{
		for (auto* it = argv + 1; it != argv + argc; ++it)
		{
			std::string temp = *it;
			 ret.emplace_back(string::trim(std::move(temp)));
		}		
	}
	return std::make_pair(current_exec, ret);
}

cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::test_switch::mode() const noexcept
{
	return m_impl->mode();
}

size_t cjm::uint128_tests::switches::test_switch::hash_code() const noexcept
{
	if (!m_impl) return 0;
	std::size_t seed = 0x1FBB0493;
	boost::hash_combine(seed, static_cast<size_t>(m_impl->mode()));
	if (auto fp = m_impl->file_path())
	{
		boost::hash_combine(seed, std::filesystem::hash_value(*fp));
	}
	return seed;	
}

std::optional<std::filesystem::path> cjm::uint128_tests::switches::test_switch::file_path() const noexcept
{
	return m_impl->file_path();
}

std::string_view cjm::uint128_tests::switches::test_switch::test_name() const noexcept
{
	return m_impl->test_name();
}

cjm::uint128_tests::switches::test_switch::test_switch(cjm::uint128_tests::switches::test_mode mode,
                                                       std::optional<std::string> parameter) : m_impl{}
{
	using namespace std::string_literals;
	m_impl = std::make_unique<test_switch_impl>(parameter.value_or(""s), mode);
}
cjm::uint128_tests::switches::test_switch::test_switch()
	: m_impl{std::make_unique<test_switch_impl>()} {}
cjm::uint128_tests::switches::test_switch::test_switch(const test_switch& other) 
	: m_impl{}
{
	m_impl = std::make_unique<test_switch_impl>(*(other.m_impl));
}

cjm::uint128_tests::switches::test_switch::test_switch(test_switch&& other) noexcept 
	: m_impl{}
{
	std::swap(m_impl, other.m_impl);
}

cjm::uint128_tests::switches::test_switch& cjm::uint128_tests::switches::test_switch::operator=(
	const test_switch& other)
{
	if (this != &other)
	{
		if (!other.m_impl) throw std::invalid_argument{ "Value to copy has been moved-from or is otherwise invalid." };
		m_impl = std::make_unique<test_switch_impl>(*(other.m_impl));
	}
	return *this;
}

cjm::uint128_tests::switches::test_switch& cjm::uint128_tests::switches::test_switch::operator=(
	test_switch&& other) noexcept
{
	if (this != &other)
	{
		std::swap(m_impl, other.m_impl);
	}
	return *this;
}



cjm::uint128_tests::switches::bad_test_switch::bad_test_switch(test_mode mode, 
	std::string_view parameter) : std::runtime_error(create_message(mode, parameter))
{
	
}

cjm::uint128_tests::switches::bad_test_switch::bad_test_switch(test_mode mode, std::string_view message,
	std::string_view parameter) 
		: std::runtime_error{create_message(mode, message, parameter)} {}

#ifdef CJM_DETECTED_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#endif
std::string cjm::uint128_tests::switches::bad_test_switch
	::create_message(test_mode mode, std::string_view parameter)
{
	auto throwing_stream = cjm::string::make_throwing_sstream<char>();

	auto get_text_from_last_two_or_combo = [](test_mode mode) -> std::string
	{
		using namespace std::string_literals;
		std::string ret;
		assert(mode == test_mode::print_environ_info || mode == test_mode::run_default_tests 
			|| mode == (test_mode::print_environ_info | test_mode::run_default_tests));
		switch (mode)   // NOLINT(clang-diagnostic-switch-enum) -- see assertion above
		{
		case test_mode::print_environ_info:
			ret = "print_environ_info"s;
			break;
		case test_mode::run_default_tests:
			ret = "run_default_tests"s;
			break;
		default:
			ret = "run_default_tests | print_environ_info"s;
			break;
		}
		return ret;
	};
	
	switch (mode)
	{
	case test_mode::unspecified: 
		throwing_stream		<< "[unspecified] is not a valid value for a test switch.  "
							<< "Parameter (if any): [" << parameter << "].";
		break;
	case test_mode::execute_unary_from_file: 
		if (parameter.empty())
		{
			throwing_stream		
				<< "Switch [execute_unary_from_file] requires the designation of a file to read from. "
				<< " No such value specified.";
		}
		else
		{
			std::optional<std::filesystem::path> x = std::nullopt;
			try
			{
				const auto copy = std::string{ parameter };
				x = std::filesystem::path{ copy};
			}
			catch (const std::exception& ex)
			{
				throwing_stream		<< "Switch [execute_unary_from_file] specified [" << parameter
									<< "] as its file path, but attempting to parse this as "
									<< "a path threw the following exception: [" << ex.what() << "].";
			}
			if (x.has_value())
			{
				if (!exists(x.value()))
				{
					throwing_stream	
						<< "Switch [execute_unary_from_file] specified [" << *x
						<< "] as its path, but the specified file is not found.";					
				}
				else
				{
					throwing_stream	
						<< "Switch [execute_unary_from_file] specified [" << parameter
						<< "] as its path, but an unknown problem occurred.";
				}
			}
			else
			{
				throwing_stream	
					<< "Switch [execute_unary_from_file] specified [" << parameter
					<< "] as its path, but an unknown problem occurred.";
			}			
		}
		break;
	case test_mode::execute_binary_from_file: 
		if (parameter.empty())
		{
			throwing_stream	
				<< "Switch [execute_binary_from_file] requires the designation "
				<< "of a file to read from.  No such value specified.";
		}
		else
		{

			std::optional<std::filesystem::path> x = std::nullopt;
			try
			{
				x = std::filesystem::path( parameter);
			}
			catch (const std::exception& ex)
			{
				throwing_stream	
					<< "Switch [execute_binary_from_file] specified ["
					<< parameter << "] as its file path, but attempting "
					<< "to parse this as a path threw the following exception: "
					<< "[" << ex.what() << "].";
			}
			if (x.has_value())
			{
				if (!exists(x.value()))
				{
					throwing_stream	
						<< "Switch [execute_binary_from_file] specified ["
						<< *x << "] as its path, but the specified file is not found.";
				}
				else
				{
					throwing_stream	
						<< "Switch [execute_binary_from_file] specified ["
						<< parameter << "] as its path, but an unknown problem occurred.";
				}
			}
			else
			{
				throwing_stream		
					<< "Switch [execute_binary_from_file] specified [" << parameter
					<< "] as its path, but an unknown problem occurred.";
			}
		}
		break;
	case test_mode::print_environ_info:
	case test_mode::run_default_tests: //flag enum  // NOLINT(clang-diagnostic-switch)
		// ReSharper disable once CppRedundantParentheses //note enum is a "flags" type enum.
	case (test_mode::print_environ_info | test_mode::run_default_tests):   // NOLINT(clang-diagnostic-switch)  
		{
			std::string text = get_text_from_last_two_or_combo(mode);
			if (!parameter.empty())
			{
				throwing_stream	
					<< "[" << text << "] switch came with parameter: [" << parameter
					<< " but this/these switches do not permit or require parameters.";
			}
			else
			{
				throwing_stream		<< "[" << text << "] "
									<< "switch came but there is an unknown problem with it.";
			}
		}
		break;
	default:  // NOLINT(clang-diagnostic-covered-switch-default) // degenerate case
		throwing_stream	
			<< "[0x" << std::hex << std::setw(std::numeric_limits<test_mode_underlying_t>::digits / 4)
			<< std::setfill('0') << static_cast<test_mode_underlying_t>(mode)
			<< "] is the bit pattern associated with the switches supplied, "
			<< "but there is an unknown problem with it.";
		if (parameter.empty())
		{
			throwing_stream << "  No parameters were supplied.";
		}
		else
		{
			throwing_stream 
				<< " The following parameter was supplied: [" 
				<< parameter << "].";
		}
		break;
	}
	return throwing_stream.str();
}
#ifdef CJM_DETECTED_GCC
#pragma GCC diagnostic pop
#endif

std::string cjm::uint128_tests::switches::bad_test_switch::create_message(test_mode mode, 
	std::string_view message, std::string_view parameter)
{
	auto strm = cjm::string::make_throwing_sstream<char>();
	if (message.empty() && parameter.empty())
	{
		strm
			<< "There is an unknown problem with the supplied switch: ["
			<< get_text_any_mode(mode) << "].";
	}
	else if (message.empty())
	{
		strm
			<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
			<< "], that came with parameter: [" << parameter << "].";
	}
	else if (parameter.empty())
	{
		strm
			<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
			<< "]: \"" << message << "\".";
	}
	else
	{
		strm
			<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
			<< "], that came with parameter [" << parameter << "]: \"" << message << "\".";
	}
	return strm.str();
}

std::string cjm::uint128_tests::switches::get_usage_examples_all_modes()
{
	auto stream = string::make_throwing_sstream<char>();
	stream << "Usage examples for all switches: " << newl;
	for (const auto& itm : individual_specified_flags)
	{
		append_usage_example(stream, itm);
	}
	stream << "DONE with usage examples." << newl << newl;
	return string::move_extract_string(std::move(stream));
}

std::stringstream& cjm::uint128_tests::switches::append_usage_example(std::stringstream& ss, test_mode mode)
{
	const auto mode_text = get_text_for_indiv_flag(mode).value_or("Unknown Mode"sv);

	switch (mode)
	{
	default:
	case test_mode::unspecified:
		ss << "switch [" << mode_text << "] is not recognized." << newl;
		break;
	case test_mode::execute_unary_from_file:
		ss << "--" << mode_text << " \t\t\t my_unary_operations_file.txt" << newl;
		break;
	case test_mode::execute_binary_from_file:
		ss << "--" << mode_text << " \t\t\t my_binary_operations_file.txt" << newl;
		break;
	case test_mode::run_specific_test:
		ss << "--" << mode_text << " \t\t\t name_of_test_to_run" << newl;
		break;
	case test_mode::print_environ_info:
	case test_mode::help:
	case test_mode::list_tests:
	case test_mode::run_default_tests:
		ss << "--" << mode_text << " \t\t\t" << newl;
		break;
	}
	return ss;
}

std::string cjm::uint128_tests::switches::get_usage_example(test_mode mode)
{
	auto stream = string::make_throwing_sstream<char>();
	append_usage_example(stream, mode);
	return string::move_extract_string(std::move(stream));
}

std::weak_ordering cjm::uint128_tests::switches::operator<=>(const test_switch& lhs, 
                                                             const test_switch& rhs) noexcept
{
	bool l_good = static_cast<bool>(lhs.m_impl);
	bool r_good = static_cast<bool>(rhs.m_impl);
	if (!l_good  || !r_good)
	{
		if (l_good == r_good)
		{
			return std::weak_ordering::equivalent;
		}
		return l_good ? std::weak_ordering::greater : std::weak_ordering::less;
	}

	const std::weak_ordering mode_ordering = static_cast<test_mode_underlying_t>(lhs.m_impl->mode()) <=> 
		static_cast<test_mode_underlying_t>(rhs.m_impl->mode());
	if (mode_ordering == std::weak_ordering::equivalent)
	{
		
		if (lhs.m_impl->file_path() == rhs.m_impl->file_path())
			return std::weak_ordering::equivalent;
		if (!lhs.m_impl->file_path().has_value() 
				|| !rhs.m_impl->file_path().has_value())
		{
			return lhs.m_impl->file_path().has_value() 
				? std::weak_ordering::greater 
					: std::weak_ordering::less;
		}
		const auto l_val = *(lhs.m_impl->file_path());
		const auto r_val = *(rhs.m_impl->file_path());
		if (l_val == r_val)
		{
			return std::weak_ordering::equivalent;
		}
		return  (l_val > r_val) ? 
			std::weak_ordering::greater : std::weak_ordering::less;
	}
	return mode_ordering;
}

size_t std::hash<cjm::uint128_tests::switches::test_switch>::operator()(
	const cjm::uint128_tests::switches::test_switch& key) const noexcept
{
	return key.hash_code();
}
bool cjm::uint128_tests::switches::starts_with_switch_token(cjm::uint128_tests::sv_t text) noexcept
{
	if (text.empty()) return false;
	for (auto tkn : switch_tokens)
	{
		if (text.starts_with(tkn))
			return true;
	}
	return false;
}

cjm::uint128_tests::switches::test_mode cjm::uint128_tests::switches::match_switch(sv_t text)
{

	if (!starts_with_switch_token(text))
	{
		throw not_a_switch{text};
	}

	sv_t stripped = strip_switch_token(text);
	if (stripped.empty())
	{
		throw not_a_switch{text};
	}

	auto match = std::find_if(text_mode_lookup.cbegin(), 
		text_mode_lookup.cend(),[=](const text_mode_t& pair) 
			-> bool { return boost::iequals(pair.first, stripped);});
	if (match != text_mode_lookup.cend())
	{
		return match->second;
	}
	throw unrecognized_switch{text};
}

cjm::uint128_tests::sv_t cjm::uint128_tests::switches::strip_switch_token(cjm::uint128_tests::sv_t text)
{
	for (auto tkn : switch_tokens )
	{
		if (text.starts_with(tkn))
		{
			const size_t token_length = tkn.length();
			return text.substr(token_length);
		}
	}
	return text;
}

std::string cjm::uint128_tests::switches::unrecognized_switch::create_message(
		std::string_view unrecognized_switch_text)
{
	auto str = string::make_throwing_sstream<char>();
	str << "The specified switch [" << unrecognized_switch_text << "] is not recognized." << newl;
	str << "Valid switches include: " << newl;
	for (auto [text, flag] : text_mode_lookup)
	{
		str << "\t\"-"sv << text << "\"" << newl;
	}
	return str.str();
}
std::string cjm::uint128_tests::switches::not_a_switch::create_message(
		std::string_view not_switch_text)
{
	auto str = string::make_throwing_sstream<char>();
	str << "Argument: [" << not_switch_text << "] is not a switch:" << newl;
	str << "\tSwitches begin with: \"-\", \"--\", or (on windows): \"/\" " << newl;
	return str.str();
}
cjm::uint128_tests::switches::unrecognized_switch::unrecognized_switch(const std::string &arg)
	: runtime_error(arg) {}

cjm::uint128_tests::switches::unrecognized_switch::unrecognized_switch(cjm::uint128_tests::sv_t unknown_switch_text) 
	: unrecognized_switch(create_message(unknown_switch_text)){}

cjm::uint128_tests::switches::not_a_switch::not_a_switch(const std::string& arg) 
	: runtime_error{arg}{}

cjm::uint128_tests::switches::not_a_switch::not_a_switch(sv_t not_a_switch_text) 
	: not_a_switch{create_message(not_a_switch_text)}{}

std::vector<cjm::uint128_tests::switches::test_switch> cjm::uint128_tests::switches::
	process_input(std::span<std::string> args)
{
	std::vector<test_switch> ret;
	
	auto it = args.begin();
	auto end_it = args.end();
	while (it != end_it)
	{
		std::string_view switch_text = *it;
		test_mode mode = match_switch(switch_text);
		auto parameter_needed = needs_parameter(mode).value();
		if (parameter_needed)
		{
			auto next = it + 1;
			if (next == end_it)
			{
				throw missing_parameter{mode};
			}
			std::string_view next_item = *next;
			if (starts_with_switch_token(next_item))
			{
				// next item is another switch, not required param
				//for this one
				throw missing_parameter{mode};
			}
			ret.emplace_back(mode, *next);
			it += 2;
		}
		else
		{
			ret.emplace_back(mode, std::nullopt);
			++it;
		}
	}
	return ret;
}

cjm::uint128_tests::switches::missing_parameter::missing_parameter(const std::string& txt)
	: runtime_error{txt} {}

std::string cjm::uint128_tests::switches::missing_parameter::create_message(test_mode mode)
{
	auto strm = string::make_throwing_sstream<char>();
	strm 
		<< "The specified switch [" << get_text_any_mode(mode) 
		<< "] requires a filename parameter, but none has been supplied." 
		<< newl;
	auto found = std::find_if(text_mode_lookup.cbegin(),
		text_mode_lookup.cend(),
		[=](const text_mode_t& pair)
			-> bool
			{
				return pair.second == mode;
			});
	if (found == text_mode_lookup.cend())
	{
		throw std::logic_error{
			"If we have parsed the mode and reached this point, we should be able to assume it is in the lookup."};
	}
	auto text = found->first;
	strm << "Example: " << newl;
	strm << "\t-" << text << " my_file.txt" << newl;
	return strm.str();

}

cjm::uint128_tests::switches::missing_parameter
	::missing_parameter(cjm::uint128_tests::switches::test_mode mode)
		: missing_parameter{create_message(mode)} {}

cjm::uint128_tests::switches::test_switch::~test_switch() = default;