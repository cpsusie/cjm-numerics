#ifndef CJM_TESTING_HPP_
#define CJM_TESTING_HPP_
#include <functional>
#include <boost/io/ios_state.hpp>
#include <type_traits>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cjm/numerics/numerics.hpp>
#include <sstream>
#include <optional>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <cjm/string/cjm_string.hpp>
#include <concepts>
namespace cjm
{
	namespace testing
	{
		using cout_saver = boost::io::ios_flags_saver;
		constexpr auto newl = "\n";
		void cjm_deny(bool predicateExpression);
		void cjm_assert(bool predicateExpression);

		void cjm_assert_throws(const std::function<void()>& func);

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const T& lhs, const T& rhs);

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const T& lhs, const std::optional<T>& rhs);

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const std::optional<T>& lhs, const T& rhs);

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(const T& lhs, const T& rhs, long double percent_tolerance = 0.00001L);

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(const std::optional<T>& lhs, const T& rhs, long double percent_tolerance = 0.00001L);

		template<typename T>
		void cjm_assert_nullopt(const std::optional<T>& optional);

		template<std::equality_comparable T>
			requires (cjm::numerics::concepts::stream_insertable<T, char>)
		void cjm_assert_equal(const T& x, const T& y, std::string_view text_rep_of_value);
		
		template<typename Predicate>
		void cjm_assert(Predicate p);

		template<typename Predicate>
		void cjm_deny(Predicate p);

		void cjm_assert_nothrow(const std::function<void()>& func);

		template<typename Exception>
		void cjm_assert_throws(const std::function<void()>& func);

		template<typename UnsignedInteger>
		std::string convert_to_lower_hex_string(const UnsignedInteger& ui);

		template<typename UnsignedInteger>
		std::string convert_to_upper_hex_string(const UnsignedInteger& ui);
		
		template<typename UnsignedInteger>
		std::string convert_to_decimal_string(const UnsignedInteger& ui);

		class testing_failure : public std::domain_error 
		{
		public:
			explicit testing_failure(std::string&& msg) : domain_error{msg}
				{}
			explicit testing_failure(const std::string& msg)
				: domain_error(msg) {}

			explicit testing_failure(const char* msg)
				: domain_error(msg) {}
		};

		class not_equal_testing_failure : public testing_failure
		{
		public:
			template<typename TValue>
				requires (std::equality_comparable<TValue> && cjm::numerics::concepts::stream_insertable<TValue, char>)
			explicit not_equal_testing_failure(const TValue& x, const TValue& y, std::string_view text_rep)
				: testing_failure(create_msg(x, y, text_rep)) {}
		
		private:
			template<typename TValue>
				requires (cjm::numerics::concepts::stream_insertable<TValue, char>)
			static std::string create_msg(const TValue& x, const TValue& y, std::string_view text_rep)
			{
				auto strm = string::make_throwing_sstream<char>();
				strm << "The value [" << x << "] does not equal the value [" << y << "]. Text representation of value: [" << text_rep << "].";
				return strm.str();
			}
		};

		template<typename Predicate>
		void cjm_assert(Predicate p)
		{
			static_assert(std::is_function_v<Predicate>, "Predicate must be a function.");
			using ret_type = decltype(p());
			static_assert(std::is_same_v<bool, ret_type>, "To be a predicate, the function needs to return true or false.");
			bool result;
			try
			{
				result = p();
			}
			catch (...)
			{
				throw testing_failure("The predicate threw an exception.");
			}
			cjm_assert(result);
		}

		template <typename Predicate>
		void cjm_deny(Predicate p)
		{
			static_assert(std::is_function_v<Predicate>, "Predicate must be a function.");
			using ret_type = decltype(p());
			static_assert(std::is_same_v<bool, ret_type>, "To be a predicate, the function needs to return true or false.");
			bool result;
			try
			{
				result = p();
			}
			catch (...)
			{
				throw testing_failure("The predicate threw an exception.");
			}
			cjm_deny(result);
		}

		template <typename Exception>
		void cjm_assert_throws(const std::function<void()>& func)
		{
			if (!func)
			{
				throw testing_failure("The specified delegate is no good.");
			}
			bool itThrew;
			bool itThrewTheWrongThing;
			try
			{
				func();
				itThrew = false;
				itThrewTheWrongThing = false;
			}
			catch (const Exception&)
			{
				itThrew = true;
				itThrewTheWrongThing = false;
			}
			catch (...)
			{
				itThrew = true;
				itThrewTheWrongThing = true;
			}
			if (!itThrew)
				throw testing_failure("The specified routine did not throw.");
			if (itThrew && itThrewTheWrongThing)
				throw testing_failure("The specified routine threw an exception but not one of the specified type.");
		}

		template <typename UnsignedInteger>
		std::string convert_to_lower_hex_string(const UnsignedInteger& ui)
		{
			static_assert(std::numeric_limits<UnsignedInteger>::is_integer && !std::numeric_limits<UnsignedInteger>::is_signed, "Type UnsignedInteger must be an unsigned integer type.");
			std::string ret;
			std::stringstream ss;
			ss << std::hex;
			ss << ui;
			ret = ss.str();
			std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
			return ret;
		}

		template<typename UnsignedInteger>
		std::string convert_to_decimal_string(const UnsignedInteger& ui)
		{
			static_assert(std::numeric_limits<UnsignedInteger>::is_integer && !std::numeric_limits<UnsignedInteger>::is_signed, "Type UnsignedInteger must be an unsigned integer type.");
			std::string ret;
			std::stringstream ss;
			ss << std::dec;
			ss << ui;
			ret = ss.str();
			return ret;
		}

		template <typename UnsignedInteger>
		std::string convert_to_upper_hex_string(const UnsignedInteger& ui)
		{
			static_assert(std::numeric_limits<UnsignedInteger>::is_integer && !std::numeric_limits<UnsignedInteger>::is_signed, "Type UnsignedInteger must be an unsigned integer type.");
			std::string ret;
			std::stringstream ss;
			ss << std::hex;
			ss << ui;
			ret = ss.str();
			std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) -> unsigned char {return static_cast<unsigned char>(toupper(c)); });
			return ret;
		}

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const T& lhs, const T& rhs)
		{
			using val_type_t = std::remove_cvref_t<std::remove_const_t<T>>;
			using namespace std::string_view_literals;
			std::strong_ordering res = lhs <=> rhs;
			if (res == std::strong_ordering::equal)
				return;
			val_type_t difference;
			const val_type_t* greater;
			std::string_view greater_variable, lesser_variable;
			if (res == std::strong_ordering::less)
			{
				difference = (rhs - lhs);
				greater = &rhs;
				greater_variable = "rhs"sv;
				lesser_variable = "lhs"sv;
			}
			else
			{
				difference = (lhs - rhs);
				greater = &lhs;
				greater_variable = "lhs"sv;
				lesser_variable = "rhs"sv;
			}
			res == std::strong_ordering::less ? (rhs - lhs) : (lhs-rhs);
			auto strm = string::make_throwing_sstream<char>();
			constexpr auto hex_width = std::numeric_limits<val_type_t>::digits / 4;
			static_assert(hex_width > 0);
			strm
				<< "Test failed.  [0x" << std::hex << std::setw(hex_width)
				<< std::setfill('0') << lhs << "] does not equal [0x"
				<< std::hex << std::setw(hex_width) << std::setfill('0')
				<< rhs << "]." << "  " << greater_variable
				<< " is greater than " << lesser_variable << " by [0x"
				<< std::hex << std::setw(hex_width)
				<< std::setfill('0') << difference
				<< "], or (in decimal): \"" << std::dec << difference
				<< "\"." << "  Percent difference: " <<
					(100.0L * (static_cast<long double>(difference) /
						static_cast<long double>(*greater)))
				<< "%." << newl;
			throw testing_failure{strm.str()};
		}

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(const T& lhs, const T& rhs, long double percent_tolerance)
		{
			if (percent_tolerance <= 0 || percent_tolerance >= 1 )
				throw testing_failure{"percent_tolerance must be between 0 and 1 EXCLUSIVELY."};
			using val_type_t = std::remove_cvref_t<std::remove_const_t<T>>;
			using namespace std::string_view_literals;
			
			constexpr auto hex_width = std::numeric_limits<val_type_t>::digits / 4;
			constexpr auto float_prec = 64;
			
			std::strong_ordering res = lhs <=> rhs;
			if (res == std::strong_ordering::equal)
				return;
			val_type_t difference;
			const val_type_t* greater;
			std::string_view greater_variable, lesser_variable;
			if (res == std::strong_ordering::less)
			{
				difference = (rhs - lhs);
				greater = &rhs;
				greater_variable = "rhs"sv;
				lesser_variable = "lhs"sv;
			}
			else
			{
				difference = (lhs - rhs);
				greater = &lhs;
				greater_variable = "lhs"sv;
				lesser_variable = "rhs"sv;
			}

			const long double percent_difference = static_cast<long double>(difference) / static_cast<long double>(*greater);
			const long double percent_difference_print = percent_difference * 100.0L;
			const long double percent_tolerance_print = percent_tolerance * 100.0L;
			if (percent_difference <= percent_tolerance)
			{
				auto saver = cout_saver{std::cout};
				std::cout << "[0x" << std::hex << std::setw(hex_width)
						<< std::setfill('0') << lhs << "] does not equal [0x"
						<< std::hex << std::setw(hex_width) << std::setfill('0')
						<< rhs << "].  The values are within the specified percent"
						<< " tolerance of: \"" << std::dec << std::setprecision(float_prec)
						<< percent_tolerance_print << std::fixed
						<< "%\".  Actual percent difference: \"" << std::dec
						<< std::setprecision(float_prec) << std::fixed << percent_difference_print << "\"." << newl;
				return;
			}

			res == std::strong_ordering::less ? (rhs - lhs) : (lhs-rhs);
			auto strm = string::make_throwing_sstream<char>();

			strm
					<< "Test failed.  Values not within specified percent tolerance of: \""
					<< std::dec << std::setprecision(float_prec) << percent_tolerance_print << "%\"."
					<< "  [0x" << std::hex << std::setw(hex_width)
					<< std::setfill('0') << lhs << "] does not equal [0x"
					<< std::hex << std::setw(hex_width) << std::setfill('0')
					<< rhs << "]." << "  " << greater_variable
					<< " is greater than " << lesser_variable << " by [0x"
					<< std::hex << std::setw(hex_width)
					<< std::setfill('0') << difference
					<< "], or (in decimal): \"" << std::dec << difference
					<< "\"." << "  Percent difference: "
					<< std::dec << std::setprecision(float_prec) << std::fixed
					<< percent_difference_print << "%." << newl;
			throw testing_failure{strm.str()};
		}

		template <numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(const std::optional<T>& lhs, const T& rhs, long double percent_tolerance)
		{
			if (!lhs.has_value()) throw std::invalid_argument{ "lhs is std::nullopt." };
			return cjm_assert_close_enough(*lhs, rhs, percent_tolerance);
		}

		template <numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(std::optional<T>&& lhs, const T& rhs, long double percent_tolerance)
		{
			if (!lhs.has_value()) throw std::invalid_argument{ "lhs is std::nullopt." };
			return cjm_assert_close_enough(*lhs, rhs, percent_tolerance);
		}

		template<typename T>
		void cjm_assert_nullopt(const std::optional<T>& optional)
		{
			if (optional.has_value()) throw testing_failure{ "The specified parameter is not std::nullopt." };
		}

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const T& lhs, const std::optional<T>& rhs)
		{
			if (!rhs.has_value()) throw testing_failure{ "The right hand parameter is std::nullopt." };
			cjm_assert_equal(lhs, *rhs);
		}

		template <numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_equal(const std::optional<T>& lhs, const T& rhs)
		{
			if (!lhs.has_value()) throw testing_failure{ "The left hand parameter is std::nullopt." };
			cjm_assert_equal(*lhs, rhs);
		}

		template<numerics::concepts::printable_subtractable_totally_ordered T>
		void cjm_assert_close_enough(const T& lhs, const std::optional<T>& rhs, long double percent_tolerance)
		{
			if (!rhs.has_value())  throw testing_failure{ "The right hand parameter is std::nullopt." };
			cjm_assert_close_enough(lhs, *rhs, percent_tolerance);
		}

		template<std::equality_comparable T>
			requires (cjm::numerics::concepts::stream_insertable<T, char>)
		void cjm_assert_equal(const T& x, const T& y, std::string_view text_rep_of_value)
		{
			if (x != y)
			{
				throw not_equal_testing_failure{ x, y, text_rep_of_value };
			}
		}
	}
}

#endif //CJM_TESTING_HPP_