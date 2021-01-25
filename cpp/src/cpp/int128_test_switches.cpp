#include "int128_test_switches.hpp"

#include <cassert>

namespace cjm::uint128_tests::switches
{
	struct test_switch::test_switch_impl final
	{
		friend class test_switch;

		[[nodiscard]] const std::optional<std::filesystem::path>& file_path() const noexcept { return m_parameter; }
		[[nodiscard]] test_mode mode() const noexcept { return m_mode; }
		test_switch_impl(std::string param, test_mode mode);
		test_switch_impl();
		test_switch_impl(int argc, char* argv[]);
		test_switch_impl(const test_switch_impl& other) = default;
		test_switch_impl(test_switch_impl&& other) noexcept = default;
		test_switch_impl& operator=(const test_switch_impl& other) = default;
		test_switch_impl& operator=(test_switch_impl&& other) noexcept = default;
		~test_switch_impl() = default;
	private:
		std::optional<std::filesystem::path> m_parameter;
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
	std::optional<std::filesystem::path> create_path_if_needed(cjm::uint128_tests::switches::test_mode mode, std::string_view text)
	{
		if (mode == cjm::uint128_tests::switches::test_mode::execute_binary_from_file || mode == cjm::uint128_tests::switches::test_mode::execute_unary_from_file)
		{
			return create_path(text);
		}
		return std::nullopt;
	}
}

cjm::uint128_tests::switches::test_switch::test_switch_impl::test_switch_impl(std::string param, test_mode mode)
	: m_parameter{create_path_if_needed(mode, param)}, m_mode{mode}
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
		if (needs_param && !m_parameter.has_value() || !exists(m_parameter.value()))
		{
			if (!m_parameter.has_value())
			{
				throw bad_test_switch{ mode, "Mode requires file parameter but none supplied"sv, view };
			}
			auto msg = cjm::string::make_throwing_sstream<char>();
			msg << "Mode requires file parameter referring to existing file.  Referenced file [" << m_parameter.value() << "] does not exist.";
			std::string msg_str = msg.str();
			sv_t msg_view = msg_str;
			throw bad_test_switch(mode, msg_str, param);
		}
		
		if (!needs_param && m_parameter.has_value())
		{
			throw bad_test_switch{ mode, "The supplied switch does not take a parameter, but one was supplied."sv, view };
		}
		
	}
}

cjm::uint128_tests::switches::test_switch::test_switch_impl::test_switch_impl()
	: m_parameter{}, m_mode{default_mode()} {}

std::string cjm::uint128_tests::switches::get_text_any_mode(test_mode mode)
{
	auto known_text = get_text_for_flag_combo(mode);
	if (known_text.has_value())
	{
		return std::string{ known_text.value() };
	}
	auto str = cjm::string::make_throwing_sstream<char>();
	constexpr size_t digits = std::numeric_limits<test_mode_underlying_t>::digits;
	constexpr size_t width = digits / 4;
	static_assert(width >= 1, "Unexpected width for underlying type of test_mode enum class.");
	str		<< "Unrecognized test switch has bit pattern: [0x" << std::hex << std::setw(width)
		<< std::setfill('0') << static_cast<test_mode_underlying_t>(mode) << "].";
	return str.str();
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

const std::optional<std::filesystem::path>& cjm::uint128_tests::switches::test_switch::file_path() const noexcept
{
	return m_impl->file_path();
}

cjm::uint128_tests::switches::test_switch::test_switch() : m_impl{std::make_unique<test_switch_impl>()} {}
cjm::uint128_tests::switches::test_switch::test_switch(const test_switch& other) : m_impl{}
{
	m_impl = std::make_unique<test_switch_impl>(*(other.m_impl));
}

cjm::uint128_tests::switches::test_switch::test_switch(test_switch&& other) noexcept : m_impl{}
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

cjm::uint128_tests::switches::bad_test_switch::bad_test_switch(test_mode mode, std::string_view parameter) : std::runtime_error(create_message(mode, parameter))
{
	
}

cjm::uint128_tests::switches::bad_test_switch::bad_test_switch(test_mode mode, std::string_view message,
	std::string_view parameter) : std::runtime_error{create_message(mode, message, parameter)} {}

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
			throwing_stream		<< "Switch [execute_unary_from_file] requires the designation of a file to read from. "
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
					throwing_stream		<< "Switch [execute_unary_from_file] specified [" << *x
										<< "] as its path, but the specified file is not found.";					
				}
				else
				{
					throwing_stream		<< "Switch [execute_unary_from_file] specified [" << parameter
										<< "] as its path, but an unknown problem occurred.";
				}
			}
			else
			{
				throwing_stream		<< "Switch [execute_unary_from_file] specified [" << parameter
									<< "] as its path, but an unknown problem occurred.";
			}			
		}
		break;
	case test_mode::execute_binary_from_file: 
		if (parameter.empty())
		{
			throwing_stream		<< "Switch [execute_binary_from_file] requires the designation "
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
				throwing_stream		<< "Switch [execute_binary_from_file] specified ["
									<< parameter << "] as its file path, but attempting "
									<< "to parse this as a path threw the following exception: "
									<< "[" << ex.what() << "].";
			}
			if (x.has_value())
			{
				if (!exists(x.value()))
				{
					throwing_stream		<< "Switch [execute_binary_from_file] specified ["
										<< *x << "] as its path, but the specified file is not found.";
				}
				else
				{
					throwing_stream		<< "Switch [execute_binary_from_file] specified ["
										<< parameter << "] as its path, but an unknown problem occurred.";
				}
			}
			else
			{
				throwing_stream		<< "Switch [execute_binary_from_file] specified [" << parameter
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
				throwing_stream		<< "[" << text << "] switch came with parameter: [" << parameter
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
		throwing_stream		<< "[0x" << std::hex << std::setw(std::numeric_limits<test_mode_underlying_t>::digits / 4)
							<< std::setfill('0') << static_cast<test_mode_underlying_t>(mode)
							<< "] is the bit pattern associated with the switches supplied, "
							<< "but there is an unknown problem with it.";
		if (parameter.empty())
		{
			throwing_stream << "  No parameters were supplied.";
		}
		else
		{
			throwing_stream << " The following parameter was supplied: [" << parameter << "].";
		}
		break;
	}
	return throwing_stream.str();
}

std::string cjm::uint128_tests::switches::bad_test_switch::create_message(test_mode mode, std::string_view message,
	std::string_view parameter)
{
	auto strm = cjm::string::make_throwing_sstream<char>();
	auto mode_rep = get_text_for_flag_combo(mode);
	if (message.empty() && parameter.empty())
	{
		strm	<< "There is an unknown problem with the supplied switch: ["
				<< get_text_any_mode(mode) << "].";
	}
	else if (message.empty())
	{
		strm	<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
				<< "], that came with parameter: [" << parameter << "].";
	}
	else if (parameter.empty())
	{
		strm	<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
				<< "]: \"" << message << "\".";
	}
	else
	{
		strm	<< "There is a problem with the supplied switch: [" << get_text_any_mode(mode)
				<< "], that came with parameter [" << parameter << "]: \"" << message << "\".";
	}
	return strm.str();
}

std::weak_ordering cjm::uint128_tests::switches::operator<=>(const test_switch& lhs, const test_switch& rhs) noexcept
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

	const std::weak_ordering mode_ordering = static_cast<test_mode_underlying_t>(lhs.m_impl->mode()) <=> static_cast<test_mode_underlying_t>(rhs.m_impl->mode());
	if (mode_ordering == std::weak_ordering::equivalent)
	{
		
		if (lhs.m_impl->file_path() == rhs.m_impl->file_path())
			return std::weak_ordering::equivalent;
		if (!lhs.m_impl->file_path().has_value() || !rhs.m_impl->file_path().has_value())
		{
			return lhs.m_impl->file_path().has_value() ? std::weak_ordering::greater : std::weak_ordering::less;
		}
		const auto& l_val = *(lhs.m_impl->file_path());
		const auto& r_val = *(rhs.m_impl->file_path());
		if (l_val == r_val)
		{
			return std::weak_ordering::equivalent;
		}
		return  (l_val > r_val) ? std::weak_ordering::greater : std::weak_ordering::less;
	}
	return mode_ordering;
}

size_t std::hash<cjm::uint128_tests::switches::test_switch>::operator()(
	const cjm::uint128_tests::switches::test_switch& key) const noexcept
{
	return key.hash_code();
}
