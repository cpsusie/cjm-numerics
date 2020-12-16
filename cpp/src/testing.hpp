#pragma once
#ifndef CJM_TESTSETTINGS_HPP 
#define CJM_TESTSETTINGS_HPP
#include "uint128_t.h"
#include <functional>
#include <type_traits>
#include <algorithm>
namespace cjm
{
	namespace testing
	{
		void cjm_deny(bool predicateExpression);
		void cjm_assert(bool predicateExpression);

		void cjm_assert_throws(const std::function<void()>& func);

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
			explicit testing_failure(const std::string& _Message)
				: domain_error(_Message) {}

			explicit testing_failure(const char* _Message)
				: domain_error(_Message) {}
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
	}
}

#endif