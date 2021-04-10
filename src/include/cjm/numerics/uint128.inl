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

//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CJM_UINT128_INL_
#define CJM_UINT128_INL_
#include <cjm/numerics/uint128.hpp>
#include <algorithm>
#include <cjm/string/cjm_string.hpp>
#include <cwctype>

constexpr size_t std::hash<cjm::numerics::uint128>::operator()(const cjm::numerics::uint128& keyVal) const noexcept
{
	return keyVal.hash_code();
}

namespace cjm
{
	namespace numerics
	{


		namespace uint128_literals
		{
			template<char... Chars>
			requires (sizeof...(Chars) > 0)
				constexpr uint128 operator"" _u128()
			{
				constexpr std::optional<uint128> result = uint128_literals::lit_helper::parse_literal<uint128,
					Chars...>();
				static_assert(result.has_value(), "This literal is not a valid decimal or hexadecimal uint128_t.");
				return *result;
			}
		}
		template <typename Char, typename CharTraits, typename Allocator>
		requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
			std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v)
		{
			using ios = std::basic_ios<Char, CharTraits>;
			using iosflags = typename ios::fmtflags;
			iosflags flags = os.flags();
			auto rep = uint128::to_string<Char, CharTraits, Allocator>(v, flags);
			std::streamsize width = os.width(0);
			using str_size_t = std::remove_cvref_t<std::make_signed_t<decltype(rep.size())>>;
			using common_type_t = std::common_type_t<str_size_t, std::streamsize>;
			if (static_cast<common_type_t>(width) > static_cast<common_type_t>(rep.size()))
			{
				const auto width_less_size = static_cast<str_size_t>(static_cast<common_type_t>(width)
					- static_cast<common_type_t>(rep.size()));
				iosflags adjustfield = flags & ios::adjustfield;
				if (adjustfield == ios::left)
				{
					rep.append(width_less_size, os.fill());
				}
				else if (adjustfield == ios::internal && (flags & ios::showbase)
					&& (flags & ios::basefield) == ios::hex && v != 0)
				{
					rep.insert(2, width_less_size + 2, os.fill());
				}
				else
				{
					rep.insert(0, width_less_size, os.fill());
				}
			}
			return os << rep;
		}

		template <typename Char, typename CharTraits>
		requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
			std::basic_istream<Char, CharTraits>& operator>>(std::basic_istream<Char,
				CharTraits>& is, uint128& v)
		{
			v = 0;
			if (is.bad() || is.fail())
			{
				std::cerr << "Stream already in bad state upon attempt to extract uint128.";
				is.setstate(std::ios_base::failbit | std::ios_base::badbit);
				return is;
			}
			if (is.eof())
			{
				std::cerr << "Failed to extract uint128 from stream: already at eof.\n";
				is.setstate(std::ios_base::failbit);
			}
			std::basic_string<Char, CharTraits> str;
			if constexpr ((!cjm::numerics::is_windows) &&
				cjm::numerics::concepts::utf_character<Char>)
			{
				Char c{};
				do
				{
					if (is.good() && !is.eof() && is.peek() != CharTraits::eof())
					{
						is.get(c);
						if (!is.bad() && !is.fail())
						{
							if (!std::isspace<char>(
								static_cast<char>(static_cast<unsigned char>(c)), std::locale("")))
								str.push_back(c);
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
				if (str.empty())
				{
					is.setstate(std::ios_base::failbit);
					return is;
				}
			}
			else
			{
				is >> str;
			}

			if (is.bad() || is.fail())
			{
				return is;
			}
			try
			{
				v = uint128::make_from_string(str);
				return is;
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Error parsing stream to uint128: [" << ex.what() << "].\n";
				is.setstate(std::ios_base::failbit);
			}
			return is;
		}



		template<typename Char, typename CharTraits, typename Allocator>
		requires (cjm::numerics::concepts::utf_character<Char>) &&
			(cjm::numerics::concepts::char_with_traits_and_allocator<Char,
				CharTraits, Allocator>)
			std::basic_string<Char, CharTraits, Allocator> uint128::to_string(uint128 item,
				std::ios_base::fmtflags flags)
		{
			if constexpr (is_windows)
			{
				constexpr char zero_char_temp = '0';
				constexpr Char zero_char = static_cast<Char>(zero_char_temp);
				uint128 div;
				int div_base_log;
				switch (flags & std::basic_ios<Char, CharTraits>::basefield)
				{
				case std::basic_ios<Char, CharTraits>::hex:
					div = 0x1000000000000000;  // 16^15
					div_base_log = 15;
					break;
				case std::basic_ios<Char, CharTraits>::oct:
					div = 01000000000000000000000;  // 8^21
					div_base_log = 21;
					break;
				default:  // std::ios::dec
					div = 10000000000000000000u;  // 10^19
					div_base_log = 19;
					break;
				}
				std::basic_stringstream<Char, CharTraits, Allocator> os;
				std::ios_base::fmtflags copyMask = std::basic_ios<Char, CharTraits>::basefield | std::basic_ios<Char,
					CharTraits>::showbase | std::basic_ios<Char, CharTraits>::uppercase;
				os.setf(flags & copyMask, copyMask);
				uint128 high = item;
				uint128 low = 0;
				best_safe_div_mod(high, div, &high, &low);
				uint128 mid = 0;
				best_safe_div_mod(high, div, &high, &mid);
				if (high.low_part() != 0)
				{
					os << high.low_part();
					os << std::noshowbase << std::setfill(zero_char) << std::setw(div_base_log);
					os << mid.low_part();
					os << std::setw(div_base_log);
				}
				else if (mid.low_part() != 0)
				{
					os << mid.low_part();
					os << std::noshowbase << std::setfill(zero_char) << std::setw(div_base_log);
				}
				os << low.low_part();
				return os.str();
			}
			else //cludge for linux and clang which aren't nice enough to provide facets for the utf strings
			{
				std::string converted_narrow = uint128::to_string<char, std::char_traits<char>, std::allocator<char>>(item, flags);
				std::basic_string<Char, CharTraits, Allocator> ret;
				ret.reserve(converted_narrow.size());
				std::transform(converted_narrow.cbegin(), converted_narrow.cend(), std::back_inserter(ret), [](char c) -> Char {return static_cast<Char>(c); });
				return ret;
			}
		}

		template<typename Char, typename CharTraits, typename Allocator>
		requires (!cjm::numerics::concepts::utf_character<Char>) &&
			(cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>)
			std::basic_string<Char, CharTraits, Allocator> uint128::to_string(uint128 item, std::ios_base::fmtflags flags)
		{
			constexpr char zero_char_temp = '0';
			constexpr Char zero_char = static_cast<Char>(zero_char_temp);
			uint128 div;
			int div_base_log;
			switch (flags & std::basic_ios<Char, CharTraits>::basefield)
			{
			case std::basic_ios<Char, CharTraits>::hex:
				div = 0x1000000000000000;  // 16^15
				div_base_log = 15;
				break;
			case std::basic_ios<Char, CharTraits>::oct:
				div = 01000000000000000000000;  // 8^21
				div_base_log = 21;
				break;
			default:  // std::ios::dec
				div = 10000000000000000000u;  // 10^19
				div_base_log = 19;
				break;
			}
			std::basic_stringstream<Char, CharTraits, Allocator> os;
			std::ios_base::fmtflags copyMask = std::basic_ios<Char, CharTraits>::basefield | std::basic_ios<Char, CharTraits>::showbase | std::basic_ios<Char, CharTraits>::uppercase;
			os.setf(flags & copyMask, copyMask);
			uint128 high = item;
			uint128 low = 0;
			best_safe_div_mod(high, div, &high, &low);
			uint128 mid = 0;
			best_safe_div_mod(high, div, &high, &mid);
			if (high.low_part() != 0)
			{
				os << high.low_part();
				os << std::noshowbase << std::setfill(zero_char) << std::setw(div_base_log);
				os << mid.low_part();
				os << std::setw(div_base_log);
			}
			else if (mid.low_part() != 0)
			{
				os << mid.low_part();
				os << std::noshowbase << std::setfill(zero_char) << std::setw(div_base_log);
			}
			os << low.low_part();
			return os.str();
		}

		constexpr uint128 uint128::make_from_bytes_little_endian(byte_array bytes) noexcept
		{
			using ret_t = uint128;
			constexpr size_t byte_count = sizeof(ret_t);
			static_assert(byte_count == bytes.size());
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"mixed endian not supported by this library.");

			if constexpr (std::endian::native == std::endian::little)
			{
				return make_from_bytes_native(bytes);
			}
			else // constexpr (std::endian::native == std::endian::big)
			{
				byte_array reversed{};
				for (size_t i = 0; i < reversed.size(); ++i)
				{
					reversed[reversed.size() - 1 - i] = bytes[i];
				}
				return make_from_bytes_native(reversed);
			}
		}

		constexpr uint128 uint128::make_from_bytes_big_endian(byte_array bytes) noexcept
		{
			using ret_t = uint128;
			constexpr size_t byte_count = sizeof(ret_t);
			static_assert(byte_count == bytes.size());
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"mixed endian not supported by this library.");

			if  constexpr (std::endian::native == std::endian::big)
			{
				return make_from_bytes_native(bytes);
			}
			else //constexpr (std::endian::native == std::endian::little)
			{
				byte_array reversed{};
				for (size_t i = 0; i < reversed.size(); ++i)
				{
					reversed[reversed.size() - 1 - i] = bytes[i];
				}
				return make_from_bytes_native(reversed);
			}
		}

		constexpr uint128 uint128::make_uint128(std::uint64_t high, std::uint64_t low) noexcept
		{
			return uint128(high, low);
		}

		constexpr std::optional<uint128::divmod_result_t> uint128::try_div_mod(uint128 dividend,
			uint128 divisor) noexcept
		{
			if (divisor == 0)
			{
				return std::nullopt;
			}
			return std::make_optional(unsafe_div_mod(dividend, divisor));
		}

		constexpr uint128::divmod_result_t uint128::unsafe_div_mod(uint128 dividend, uint128 divisor) noexcept //NOLINT (bugprone-exception-escape)
		{   //exception can only be thrown if natuint128_t is NOT a built-in (i.e. is alias for uint128)
			//AND calc_mode is intrinsic_u128: this is not possible .. thus linting is suppressed
			static_assert(calculation_mode != uint128_calc_mode::intrinsic_u128 || !std::is_same_v<natuint128_t, uint128>,
				"It should not be possible for calc_mode to be intrinsic and have natuint128_t be set as uint128/");
			assert(divisor != 0);
			if (std::is_constant_evaluated())
			{
				uint128 quotient{};
				uint128 remainder{};
				unsafe_constexpr_div_mod_impl(dividend, divisor, &quotient, &remainder);
				return divmod_result_t{ quotient, remainder };
			}
			// ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			//(I use this style with explicit else when using if constexpr or std::is_constant_evaluated)
			else  // ReSharper disable CppUnreachableCode				
			{
				if constexpr (calculation_mode == uint128_calc_mode::
					intrinsic_u128)
				{
					return divmod_result_t
					{
						static_cast<natuint128_t>(dividend) /
								static_cast<natuint128_t>(divisor),
						static_cast<natuint128_t>(dividend) %
								static_cast<natuint128_t>(divisor)
					};
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
				{
					uint128 quotient{};
					uint128 remainder{};
					uint128::div_mod_msc_x64_impl(dividend, divisor, &quotient, &remainder);
					return divmod_result_t{ quotient, remainder };
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					uint128 quotient{};
					uint128 remainder{};
					unsafe_constexpr_div_mod_impl(dividend, divisor, &quotient, &remainder);
					return divmod_result_t{ quotient, remainder };
				}
			}// ReSharper restore CppUnreachableCode

		}


		constexpr uint128::uint128() noexcept : m_limbs{} {}

		constexpr uint128::divmod_result_t uint128::div_mod(uint128 dividend, uint128 divisor)
		{
			if (divisor == 0) throw std::domain_error("Division and/or modulus by zero is forbidden.");
			return unsafe_div_mod(dividend, divisor);
		}

		constexpr uint128::uint128(int v) noexcept
			: m_limbs{ static_cast<std::int64_t>(v) } {}
		constexpr uint128::uint128(unsigned int v) noexcept
			: m_limbs{ static_cast<int_part>(v) } {}
		constexpr uint128::uint128(long v) noexcept
			: m_limbs{ static_cast<std::int64_t>(v) } {}
		constexpr uint128::uint128(unsigned long v) noexcept
			: m_limbs{ static_cast<std::uint64_t>(v) } {}
		constexpr uint128::uint128(long long v) noexcept
			: m_limbs{ static_cast<std::int64_t>(v) } {}
		constexpr uint128::uint128(unsigned long long v) noexcept
			: m_limbs{ static_cast<std::uint64_t>(v) } {}
		constexpr uint128& uint128::operator=(int v) noexcept
		{
			m_limbs.m_low = static_cast<std::int64_t>(v);
			m_limbs.m_high = v < 0 ? std::numeric_limits<std::uint64_t>::max() : 0u;
			return *this;
		}

		constexpr uint128& uint128::operator=(unsigned int v) noexcept
		{
			m_limbs.m_low = static_cast<int_part>(v);
			m_limbs.m_high = 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(long v) noexcept
		{
			m_limbs.m_low = static_cast<int_part>(v);
			m_limbs.m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(unsigned long v) noexcept
		{
			m_limbs.m_low = static_cast<int_part>(v);
			m_limbs.m_high = 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(long long v) noexcept
		{
			m_limbs.m_low = static_cast<int_part>(v);
			m_limbs.m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(unsigned long long v) noexcept
		{
			m_limbs.m_low = static_cast<int_part>(v);
			m_limbs.m_high = 0;
			return *this;
		}

		//converting ctors from floating point types
		inline uint128::uint128(float f) noexcept : uint128()
		{
			auto temp = internal::make_from_floating_point(f);
			m_limbs.m_high = temp.m_limbs.m_high;
			m_limbs.m_low = temp.m_limbs.m_low;
		}

		inline uint128::uint128(double d) noexcept : uint128()
		{
			auto temp = internal::make_from_floating_point(d);
			m_limbs.m_high = temp.m_limbs.m_high;
			m_limbs.m_low = temp.m_limbs.m_low;
		}

		inline uint128::uint128(long double d) noexcept : uint128()
		{
			auto temp = internal::make_from_floating_point(d);
			m_limbs.m_high = temp.m_limbs.m_high;
			m_limbs.m_low = temp.m_limbs.m_low;
		}

		template<typename Chars, typename CharTraits, typename Allocator>
		requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
			uint128 uint128::make_from_string(const std::basic_string<Chars, CharTraits, Allocator>& parse_me)
		{
			return make_from_string(std::basic_string_view<Chars, CharTraits>{parse_me});
		}

		template<typename Chars, typename CharTraits>
		requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
			uint128 uint128::make_from_string(std::basic_string_view<Chars, CharTraits> parse_me)
		{
			using ph = u128_parsing_helper<Chars, CharTraits>;
			auto str = std::basic_string<Chars, CharTraits>{ parse_me };
			auto trimmed = ph::trim_and_strip(str);
			auto parseFormat = ph::get_format(trimmed);
			uint128 ret;
			switch (parseFormat)
			{
			default:
			case u128_str_format::Illegal:
				throw std::invalid_argument("Unable to parse a uint128 from the supplied string.");
			case u128_str_format::Zero:
				ret = 0;
				break;
			case u128_str_format::Decimal:
				ret = ph::parse_decimal_str(trimmed);
				break;
			case u128_str_format::Hexadecimal:
				ret = ph::parse_hex_str(trimmed);
				break;
			}
			return ret;
		}
		constexpr uint128::operator bool() const noexcept
		{
			return m_limbs.m_low || m_limbs.m_high;
		}
		constexpr uint128::operator char() const noexcept
		{
			return static_cast<char>(m_limbs.m_low);
		}
		constexpr uint128::operator signed char() const noexcept
		{
			return static_cast<signed char>(m_limbs.m_low);
		}
		constexpr uint128::operator unsigned char() const noexcept
		{
			return static_cast<unsigned char>(m_limbs.m_low);
		}

		constexpr uint128::operator char8_t() const noexcept
		{
			return static_cast<char8_t>(m_limbs.m_low);
		}

		constexpr uint128::operator char16_t() const noexcept
		{
			return static_cast<char16_t>(m_limbs.m_low);
		}
		constexpr uint128::operator char32_t() const noexcept
		{
			return static_cast<char32_t>(m_limbs.m_low);
		}
		constexpr uint128::operator wchar_t() const noexcept
		{
			return static_cast<wchar_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::int16_t() const noexcept
		{
			return static_cast<std::int16_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::uint16_t() const noexcept
		{
			return static_cast<std::uint16_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::int32_t() const noexcept
		{
			return static_cast<std::int32_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::uint32_t() const noexcept
		{
			return static_cast<std::uint32_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::int64_t() const noexcept
		{
			return static_cast<std::int64_t>(m_limbs.m_low);
		}
		constexpr uint128::operator std::uint64_t() const noexcept
		{
			return static_cast<std::uint64_t>(m_limbs.m_low);
		}

		constexpr size_t uint128::hash_code() const noexcept
		{
			return calculate_hash(m_limbs.m_high, m_limbs.m_low);
		}

		constexpr uint128& uint128::operator+=(uint128 other) noexcept
		{
			*this = (*this + other);
			return *this;
		}

		constexpr uint128& uint128::operator-=(uint128 other) noexcept
		{
			*this = (*this - other);
			return *this;
		}

		constexpr uint128& uint128::operator*=(uint128 other) noexcept
		{
			*this = (*this * other);
			return *this;
		}

		constexpr uint128& uint128::operator/=(uint128 other)
		{
			*this = (*this / other);
			return *this;
		}

		constexpr uint128& uint128::operator%=(uint128 other)
		{
			*this = (*this % other);
			return *this;
		}

		constexpr uint128 uint128::operator++(int) noexcept
		{
			uint128 tmp(*this);
			*this += 1;
			return tmp;
		}

		constexpr uint128 uint128::operator--(int) noexcept
		{
			uint128 tmp(*this);
			*this -= 1;
			return tmp;
		}

		constexpr uint128& uint128::operator<<=(int amount) noexcept
		{
			assert(amount > -1 && amount < std::numeric_limits<uint128>::digits);
			*this = *this << amount;
			return *this;
		}

		constexpr uint128& uint128::operator>>=(int amount) noexcept
		{
			assert(amount > -1 && amount < std::numeric_limits<uint128>::digits);
			*this = *this >> amount;
			return *this;
		}

		constexpr uint128& uint128::operator<<=(unsigned amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			*this = *this << amount;
			return *this;
		}

		constexpr uint128& uint128::operator>>=(unsigned amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			*this = *this >> amount;
			return *this;
		}

		constexpr uint128& uint128::operator<<=(uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			*this = *this << amount;
			return *this;
		}

		constexpr uint128& uint128::operator>>=(uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			*this = *this >> amount;
			return *this;
		}

		constexpr uint128& uint128::operator&=(uint128 other) noexcept
		{
			m_limbs.m_high &= other.m_limbs.m_high;
			m_limbs.m_low &= other.m_limbs.m_low;
			return *this;
		}

		constexpr uint128& uint128::operator|=(uint128 other) noexcept
		{
			m_limbs.m_high |= other.m_limbs.m_high;
			m_limbs.m_low |= other.m_limbs.m_low;
			return *this;
		}

		constexpr uint128& uint128::operator^=(uint128 other) noexcept
		{
			m_limbs.m_high ^= other.m_limbs.m_high;
			m_limbs.m_low ^= other.m_limbs.m_low;
			return *this;
		}

		constexpr uint128& uint128::operator++() noexcept
		{
			*this += 1;
			return *this;
		}

		constexpr uint128& uint128::operator--() noexcept
		{
			*this -= 1;
			return *this;
		}

		constexpr uint128::int_part uint128::low_part() const noexcept
		{
			return m_limbs.m_low;
		}

		constexpr uint128::int_part uint128::high_part() const noexcept
		{
			return m_limbs.m_high;
		}

		constexpr uint128::byte_array uint128::to_little_endian_arr() const noexcept //NOLINT (bugprone-exception-escape)
		{
			constexpr size_t byte_count = sizeof(byte_array);
			static_assert(byte_count == sizeof(uint128));
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"mixed endian not supported by this library.");

			if constexpr (std::endian::native == std::endian::little)
			{
				return to_bytes_native(*this);
			}
			else // constexpr (std::endian::native == std::endian::big)
			{
				byte_array temp = to_bytes_native(*this);
				byte_array reversed{};
				for (size_t i = 0; i < reversed.size(); ++i)
				{
					reversed[reversed.size() - 1 - i] = temp[i];
				}
				return reversed;
			}
		}

		constexpr uint128::byte_array uint128::to_big_endian_arr() const noexcept
		{
			constexpr size_t byte_count = sizeof(byte_array);
			static_assert(byte_count == sizeof(uint128));
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"mixed endian not supported by this library.");
			if constexpr (std::endian::native == std::endian::big)
			{
				return to_bytes_native(*this);
			}
			else //constexpr (std::endian::native == std::endian::little)
			{
				byte_array temp = to_bytes_native(*this);
				byte_array reversed{};
				for (size_t i = 0; i < reversed.size(); ++i)
				{
					reversed[reversed.size() - 1 - i] = temp[i];
				}
				return reversed;
			}
		}

		constexpr uint128::uint128(int_part high, int_part low) noexcept
			: m_limbs{ high, low } {}

		constexpr size_t uint128::calculate_hash(int_part hi, int_part low) noexcept
		{
			static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4, "Only 32 and 64 bit architecture supported.");
			size_t hash{ 0 };
			if constexpr (sizeof(size_t) == 8)
			{
#pragma warning(push)
#pragma warning (disable: 4244) //this warning pops in in msvc when compiling as 32 bit even though this if constexpr branch never taken in that case.
				hash = low + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
				hash = hi + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
#pragma warning(pop)				
			}
			else // ReSharper disable once CppUnreachableCode
			{
				size_t hi_hi = static_cast<size_t>(hi >> 32);
				size_t hi_low = static_cast<size_t>(hi & 0xffff'ffff);
				size_t low_hi = static_cast<size_t>(low >> 32);
				size_t low_low = static_cast<size_t>(low & 0xffff'ffff);

				hash = low_low + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
				hash = low_hi + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
				hash = hi_low + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
				hash = hi_hi + 0x9e37'79b9 + (hash << 6) + (hash >> 2);
			}
			return hash;
		}

		constexpr void uint128::hash_combine(size_t& seed, size_t newVal) noexcept
		{
			seed ^= (newVal + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		}

		constexpr uint128 uint128::make_from_bytes_native(byte_array bytes) noexcept
		{
			using ret_t = uint128;
			constexpr size_t byteCount = sizeof(ret_t);
			static_assert(byteCount == bytes.size());
			constexpr size_t byte_shift_size = CHAR_BIT;
			using byte = unsigned char;
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"Mixed endian is not supported.");
			if (std::is_constant_evaluated())
			{
				if constexpr (constexpr_bit_casting)
				{
					return bit_cast<uint128, byte_array>(bytes);
				}
				else if constexpr (std::endian::native == std::endian::little)
				{
					ret_t ret = 0;
					for (size_t index = 0; index < byteCount; ++index)
					{
						byte b = bytes[index];
						auto temp = static_cast<ret_t>(b);
						temp <<= static_cast<int>((index * byte_shift_size));
						ret |= temp;
					}
					return ret;
				}
				else //constexpr (std::endian::native == std::endian::big)
				{
					ret_t ret = 0;
					for (size_t index = 0; index < byteCount; ++index)
					{
						byte b = bytes[index];
						auto temp = static_cast<ret_t>(b);
						//if will become when cast negative, result of subtraction will be bigger than byteCount
						assert((byteCount - 1 - index) < byteCount);
						const auto left_shift_amount = static_cast<int>(byteCount - 1 - index);
						temp <<= left_shift_amount;
						ret |= temp;
					}
					return ret;
				}
			}
			// ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			else // ReSharper disable once CppUnreachableCode
			{
				return bit_cast<uint128, byte_array>(bytes);
			}
		}

		constexpr uint128::byte_array uint128::to_bytes_native(uint128 convert_me) noexcept //NOLINT (bugprone-exception-escape)
		{ //there is no reasonable code-logic permitting path where an exception can be thrown.
			static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
				"Mixed endian is not supported.");
			using namespace cjm::numerics::uint128_literals;
			constexpr size_t shift_amount = CHAR_BIT;
			if (std::is_constant_evaluated())
			{
				if constexpr (constexpr_bit_casting)
				{
					return bit_cast<byte_array, uint128>(convert_me);
				}
				else
				{
					if constexpr (std::endian::native == std::endian::little)
					{
						auto all_ones_in_byte = static_cast<uint128>(0xffu);
						auto ret = byte_array{};
						for (size_t index = 0; index < ret.size(); ++index)
						{
							const auto mask = all_ones_in_byte << static_cast<int>(index * shift_amount);
							const auto zeroed_out_except_relevant = mask & convert_me;
							ret[index] = static_cast<unsigned char>((zeroed_out_except_relevant >> static_cast<int>(index * shift_amount)));
						}
						return ret;
					}
					else // constexpr (std::endian::native == std::endian::big)
					{
						auto all_ones_in_byte = uint128{ 0xff00'0000'0000'0000, 0 };
						auto ret = byte_array{};
						for (size_t index = 0; index < ret.size(); ++index)
						{
							const auto mask = all_ones_in_byte >> static_cast<int>(index * shift_amount);
							const auto zeroed_out_except_relevant = mask & convert_me;
							const auto rshift_amount = static_cast<int>((ret.size() - 1 - index) * shift_amount);
							assert(rshift_amount > -1);
							ret[index] = static_cast<unsigned char>((zeroed_out_except_relevant >> rshift_amount));
						}
						return ret;
					}
				}
			} // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			else // ReSharper disable once CppUnreachableCode
			{
				return bit_cast<byte_array, uint128>(convert_me);
			}
		}


		inline uint128 uint128::lshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept
		{
			assert(shift_amount > -1 && shift_amount < std::numeric_limits<uint128>::digits);
			uint128 ret;
			if (shift_amount >= 64)
			{
				ret.m_limbs.m_high = shift_me.m_limbs.m_low;
				ret.m_limbs.m_low = 0;
				ret.m_limbs.m_high = (ret.m_limbs.m_high << (shift_amount - 64));
			}
			else
			{
				ret.m_limbs.m_high = CJM_LSHIFT128(shift_me.m_limbs.m_low, shift_me.m_limbs.m_high,
					static_cast<unsigned char>(shift_amount));
				ret.m_limbs.m_low = shift_me.m_limbs.m_low << shift_amount;
			}
			return ret;
		}

		inline uint128 uint128::rshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept
		{
			assert(shift_amount > -1 && shift_amount < std::numeric_limits<uint128>::digits);
			uint128 ret;
			if (shift_amount >= 64)
			{
				ret.m_limbs.m_high = 0;
				ret.m_limbs.m_low = shift_me.m_limbs.m_high;
				ret.m_limbs.m_low = ret.m_limbs.m_low >> (shift_amount - 64);
			}
			else
			{
				ret.m_limbs.m_low = CJM_RSHIFT128(shift_me.m_limbs.m_low, shift_me.m_limbs.m_high,
					static_cast<unsigned char>(shift_amount));
				ret.m_limbs.m_high = shift_me.m_limbs.m_high >> shift_amount;
			}
			return ret;
		}

		constexpr void uint128::constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
			uint128* quotient_ret, uint128* remainder_ret)
		{

			if (divisor == 0)
			{
				throw std::domain_error("Division and/or modulus by zero is forbidden.");
			}
			unsafe_constexpr_div_mod_impl(dividend, divisor, quotient_ret, remainder_ret);
		}

		constexpr void uint128::unsafe_constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
			uint128* quotient_ret, uint128* remainder_ret) noexcept
		{
			assert(divisor != 0);
			// Long division/modulo for uint128 implemented using the shift-subtract
		   // division algorithm adapted from:
		   // https://stackoverflow.com/questions/5386377/division-without-using
			if (divisor > dividend)
			{
				*quotient_ret = 0;
				*remainder_ret = dividend;
				return;
			}
			if (divisor == dividend)
			{
				*quotient_ret = 1;
				*remainder_ret = 0;
				return;
			}

			uint128 denominator = divisor;
			uint128 quotient = 0;

			// Left aligns the MSB of the denominator and the dividend.
			const int shift = fls(dividend) - fls(denominator);
			denominator <<= shift;

			// Uses shift-subtract algorithm to divide dividend by denominator. The
			// remainder will be left in dividend.
			for (int i = 0; i <= shift; ++i)
			{
				quotient <<= 1;
				if (dividend >= denominator)
				{
					dividend -= denominator;
					quotient |= 1;
				}
				denominator >>= 1;
			}

			*quotient_ret = quotient;
			*remainder_ret = dividend;
		}

		constexpr int uint128::fls(uint128 n)  noexcept
		{
			assert(n != 0);
			//check the high part for any set bits,
			//if so, call google's magic method on the high part
			//if not, call it on the low part.
			int_part hi = n.high_part();
			if (hi != 0)
			{
				auto flsHigh = internal::fls_int_part(hi);
				auto sum = flsHigh + int_part_bits;
				return static_cast<int>(sum);
			}
			return internal::fls_int_part(n.low_part());
		}


		template <cjm::numerics::concepts::builtin_unsigned_integer UI>
		constexpr int internal::countl_zero(UI n) noexcept
		{
			assert(n != 0);
			return std::countl_zero(n);
		}

		constexpr int internal::countl_zero(uint128 n) noexcept
		{
			assert(n != 0);
			auto hp = n.high_part();
			if (hp != 0)
			{
				return std::countl_zero(hp);
			}
			return std::numeric_limits<uint128::int_part>::digits + std::countl_zero(n.low_part());
		}

		template <typename T>
		constexpr void internal::step(T& n, int& pos, int shift) noexcept
		{
			//again, no freaking clue how this works or what it even does except it
			//works as part of fls_int_part .... google programming gods are mysterious
			//in their ways.
			if (n >= (static_cast<T>(1) << shift))
			{
				n = n >> shift;
				pos = pos | shift;
			}
		}

		constexpr int internal::fls_int_part(std::uint64_t n) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return fls_default(n);
			}
			else // ReSharper disable once CppUnreachableCode				
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64 ||
					calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					assert(n != 0);
					unsigned long result = 0;
					[[maybe_unused]] unsigned char res = CJM_BITSCAN_REV_64(&result, n);
					assert(res != 0);
					return static_cast<int>(result);
				}
				else
				{
					return fls_default(n);
				}
			}

		}

		constexpr int internal::fls_default(std::uint64_t n) noexcept
		{
			if constexpr (has_cpp20_bitops)
			{
				assert(n != 0);
				auto lz = std::countl_zero(n);
				return static_cast<int>(std::numeric_limits<std::uint64_t>::digits - 1 - lz);
			}
			else
			{
				//finding the last set bit (bitpos of most significant bit with a value of one)
					//how or why this works is totally beyond me.  It does though ... trust
					// the google gods ....
				assert(n != 0);
				int pos = 0;
				step<std::uint64_t>(n, pos, 0x20);
				auto n32 = static_cast<std::uint32_t>(n);
				step<std::uint32_t>(n32, pos, 0x10);
				step<std::uint32_t>(n32, pos, 0x08);
				step<std::uint32_t>(n32, pos, 0x04);
				return static_cast<int>((std::uint64_t{ 0x3333333322221100 } >> (n32 << 2) & 0x3) + pos);
			}
		}

		constexpr std::strong_ordering operator<=>(uint128 lhs, uint128 rhs) noexcept
		{
			return lhs == rhs ? std::strong_ordering::equal :
				((lhs > rhs) ? std::strong_ordering::greater
					: std::strong_ordering::less);
		}

		constexpr bool operator==(uint128 lhs, uint128 rhs) noexcept
		{
			return lhs.high_part() == rhs.high_part() && lhs.low_part() == rhs.low_part();
		}
		constexpr bool operator!=(uint128 lhs, uint128 rhs) noexcept
		{
			return !(lhs == rhs);
		}
		constexpr bool operator>(uint128 lhs, uint128 rhs) noexcept
		{
			return (lhs.high_part() == rhs.high_part()) ?
				(lhs.low_part() > rhs.low_part()) : (lhs.high_part() > rhs.high_part());
		}
		constexpr bool operator<(uint128 lhs, uint128 rhs) noexcept
		{
			return (lhs.high_part() == rhs.high_part()) ?
				(lhs.low_part() < rhs.low_part()) : (lhs.high_part() < rhs.high_part());
		}
		constexpr bool operator>=(uint128 lhs, uint128 rhs) noexcept
		{
			return !(lhs < rhs);
		}
		constexpr bool operator<=(uint128 lhs, uint128 rhs) noexcept
		{
			return !(lhs > rhs);
		}
		//Unary operators
		constexpr uint128 operator-(uint128 operand) noexcept
		{

			if (std::is_constant_evaluated())
			{
				using intpart = uint128::int_part;
				intpart high = ~operand.high_part();
				intpart lo = ~operand.low_part() + 1;
				if (lo == 0)
					++high; // carry
				return uint128::make_uint128(high, lo);
			}  // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement			   
			else  // ReSharper disable once CppUnreachableCode  
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return -static_cast<natuint128_t>(operand);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode ==
					uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					return (uint128{ ~operand.high_part(),
						~operand.low_part() } += uint128{ 1 });
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					using intpart = uint128::int_part;
					intpart high = ~operand.high_part();
					intpart lo = ~operand.low_part() + 1;
					if (lo == 0)
						++high; // carry
					return uint128::make_uint128(high, lo);
				}
			}

		}
		constexpr uint128 operator+(uint128 operand) noexcept
		{
			return operand;
		}
		constexpr uint128 operator~(uint128 operand) noexcept
		{
			return uint128::make_uint128(~operand.high_part(), ~operand.low_part());
		}
		constexpr bool operator!(uint128 operand) noexcept
		{
			return !(static_cast<bool>(operand));
		}
		//Logical operators
		constexpr uint128 operator&(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::make_uint128(lhs.high_part() & rhs.high_part(),
				lhs.low_part() & rhs.low_part());
		}
		constexpr uint128 operator|(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::make_uint128(lhs.high_part() | rhs.high_part(),
				lhs.low_part() | rhs.low_part());
		}
		constexpr uint128 operator^(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::make_uint128(lhs.high_part() ^ rhs.high_part(),
				lhs.low_part() ^ rhs.low_part());
		}
		//bit shift operators
		constexpr uint128 operator>>(uint128 lhs, int amount) noexcept
		{
			if (std::is_constant_evaluated())
			{
				//It is undefined behavior to attempt a shift greater than number of bits in an integral type
				assert(amount < std::numeric_limits<uint128>::digits&& amount > -1);
				// uint64_t shifts of >= 64 are undefined, so we will need some
				// special-casing.
				const auto abs_amount = static_cast<int>(math_functions::int_abs(amount));
				if (abs_amount < static_cast<int>(uint128::int_part_bits))
				{
					if (abs_amount != 0)
					{
						return
							uint128::make_uint128((lhs.high_part() >> abs_amount),
								(lhs.low_part() >> abs_amount) |
								(lhs.high_part() << (static_cast<int>(uint128::int_part_bits)
									- abs_amount)));
					}
					return lhs;
				}
				return uint128::make_uint128(0, lhs.high_part()
					>> (abs_amount - static_cast<int>(uint128::int_part_bits)));
			}
			else // ReSharper disable once CppUnreachableCode				
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return static_cast<natuint128_t>(lhs) >> amount;
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					return uint128::rshift_msvc_x64(lhs, amount);
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					//It is undefined behavior to attempt a shift greater than number of bits in an integral type
					assert(amount < std::numeric_limits<uint128>::digits
						&& amount > -1);
					// uint64_t shifts of >= 64 are undefined, so we will need some
					// special-casing.
					auto absAmount = static_cast<int>(math_functions::int_abs(amount));
					if (absAmount < static_cast<int>(uint128::int_part_bits))
					{
						if (absAmount != 0)
						{
							return uint128::make_uint128((lhs.high_part() >> absAmount),
								(lhs.low_part() >> absAmount) |
								(lhs.high_part() << (static_cast<int>(uint128::int_part_bits) - absAmount)));
						}
						return lhs;
					}
					return uint128::make_uint128(0, lhs.high_part() >> (absAmount - static_cast<int>(uint128::int_part_bits)));
				}
			}

		}
		constexpr uint128 operator<<(uint128 lhs, int amount) noexcept
		{

			if (std::is_constant_evaluated())
			{
				//It is undefined behavior to attempt a shift greater than number of bits in an integral type
				assert(amount < std::numeric_limits<uint128>::digits&& amount > -1);
				// uint64_t shifts of >= 64 are undefined, so we will need some
				// special-casing.
				const auto absAmount = static_cast<int>(math_functions::int_abs(amount));
				if (absAmount < static_cast<int>(uint128::int_part_bits))
				{
					if (absAmount != 0)
					{
						return uint128::make_uint128((lhs.high_part() << absAmount) |
							(lhs.low_part() >> (static_cast<int>(uint128::int_part_bits) - absAmount)), lhs.low_part() << absAmount);
					}
					return lhs;
				}
				return uint128::make_uint128(lhs.low_part() << (absAmount - static_cast<int>(uint128::int_part_bits)), 0);
			}
			else // ReSharper disable once CppUnreachableCode		
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return bit_cast<natuint128_t>(lhs) << amount;
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64 || calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					assert(amount < std::numeric_limits<uint128>::digits&& amount > -1);
					return uint128::lshift_msvc_x64(lhs, amount);
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					//It is undefined behavior to attempt a shift greater than number of bits in an integral type
					assert(amount < std::numeric_limits<uint128>::digits&& amount > -1);
					// uint64_t shifts of >= 64 are undefined, so we will need some
					// special-casing.
					const auto absAmount = static_cast<int>(math_functions::int_abs(amount));
					if (absAmount < static_cast<int>(uint128::int_part_bits))
					{
						if (absAmount != 0)
						{
							return uint128::make_uint128((lhs.high_part() << absAmount) |
								(lhs.low_part() >> (static_cast<int>(uint128::int_part_bits) - absAmount)), lhs.low_part() << absAmount);
						}
						return lhs;
					}
					return uint128::make_uint128(lhs.low_part()
						<< (absAmount - static_cast<int>(uint128::int_part_bits)), 0);
				}
			}
		}

		template<concepts::builtin_unsigned_integer Ui>
		constexpr uint128 operator<<(uint128 lhs, Ui amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			return (lhs << static_cast<int>(amount));
		}

		template<concepts::builtin_unsigned_integer Ui>
		constexpr uint128 operator>>(uint128 lhs, Ui amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			return (lhs >> static_cast<int>(amount));
		}

		constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits&& static_cast<int>(amount) > -1);
			return (lhs >> static_cast<int>(amount));
		}
		constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits&& static_cast<int>(amount) > -1);
			return (lhs << static_cast<int>(amount));
		}
		//arithmetic operators
		constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				auto result = uint128{ lhs.m_limbs.m_high + rhs.m_limbs.m_high,
													  lhs.m_limbs.m_low + rhs.m_limbs.m_low };
				if (result.m_limbs.m_low < lhs.m_limbs.m_low) // check for carry
				{
					++result.m_limbs.m_high;
				}
				return result;
			}  // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			else   // ReSharper disable once CppUnreachableCode
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return static_cast<natuint128_t>(lhs) + static_cast<natuint128_t>(rhs);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64 || calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					uint128 ret = 0;
					unsigned char carry_in = 0;
					unsigned char carry_out = CJM_ADDCARRY64(carry_in,
						lhs.m_limbs.m_low, rhs.m_limbs.m_low, &(ret.m_limbs.m_low));
					carry_in = carry_out;
					carry_out = CJM_ADDCARRY64(carry_in, lhs.m_limbs.m_high,
						rhs.m_limbs.m_high, &(ret.m_limbs.m_high));
					return ret;
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					auto result = uint128{ lhs.m_limbs.m_high + rhs.m_limbs.m_high,
													   lhs.m_limbs.m_low + rhs.m_limbs.m_low };
					if (result.m_limbs.m_low < lhs.m_limbs.m_low) // check for carry
					{
						++result.m_limbs.m_high;
					}
					return result;
				}
			}
		}

		constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				auto result = uint128{ lhs.m_limbs.m_high - rhs.m_limbs.m_high, lhs.m_limbs.m_low - rhs.m_limbs.m_low };
				if (lhs.m_limbs.m_low < rhs.m_limbs.m_low) // check for borrow
				{
					--result.m_limbs.m_high;
				}
				return result;
			}  // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			else  // ReSharper disable once CppUnreachableCode
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return static_cast<natuint128_t>(lhs) - static_cast<natuint128_t>(rhs);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64 || calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					uint128 ret = 0;
					unsigned char carry_in = 0;
					unsigned char carry_out = 0;
					carry_out = CJM_SUBBORROW_64(carry_in, lhs.m_limbs.m_low, rhs.m_limbs.m_low, &(ret.m_limbs.m_low));
					carry_in = carry_out;
					carry_out = CJM_SUBBORROW_64(carry_in, lhs.m_limbs.m_high, rhs.m_limbs.m_high, &(ret.m_limbs.m_high));
					return ret;
				}
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					auto result = uint128{ lhs.m_limbs.m_high - rhs.m_limbs.m_high,
						lhs.m_limbs.m_low - rhs.m_limbs.m_low };
					if (lhs.m_limbs.m_low < rhs.m_limbs.m_low) // check for borrow
					{
						--result.m_limbs.m_high;
					}
					return result;
				}
			}
		}
		constexpr uint128 operator*(uint128 lhs, uint128 rhs) noexcept
		{
			if (std::is_constant_evaluated())
			{
				using int_part = uint128::int_part;
				int_part a32 = lhs.low_part() >> uint128::int_part_bottom_half_bits;
				int_part a00 = lhs.low_part() & uint128::int_part_bottom_half_bitmask;
				int_part b32 = rhs.low_part() >> uint128::int_part_bottom_half_bits;
				int_part b00 = rhs.low_part() & uint128::int_part_bottom_half_bitmask;

				uint128 result = uint128::make_uint128(lhs.high_part() * rhs.low_part() +
					lhs.low_part() * rhs.high_part() +
					a32 * b32,
					a00 * b00);
				result += uint128(a32 * b00) << uint128::int_part_bottom_half_bits;
				result += uint128(a00 * b32) << uint128::int_part_bottom_half_bits;
				return result;
			}// NOLINT(readability-misleading-indentation)
				// ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement
			else // ReSharper disable once CppUnreachableCode 
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return static_cast<natuint128_t>(lhs) * static_cast<natuint128_t>(rhs);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64 || calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					std::uint64_t carry = 0;
					std::uint64_t low_product = CJM_UMUL128(lhs.low_part(), rhs.low_part(), &carry);
					return uint128::make_uint128(lhs.low_part() * rhs.high_part() + lhs.high_part() * rhs.low_part() + carry, low_product);
				}
				else
				{   // constexpr (calculation_mode == uint128_calc_mode::default_eval)
					using int_part = uint128::int_part;
					int_part a32 = lhs.low_part() >> uint128::int_part_bottom_half_bits;
					int_part a00 = lhs.low_part() & uint128::int_part_bottom_half_bitmask;
					int_part b32 = rhs.low_part() >> uint128::int_part_bottom_half_bits;
					int_part b00 = rhs.low_part() & uint128::int_part_bottom_half_bitmask;

					uint128 result = uint128::make_uint128(lhs.high_part() * rhs.low_part() +
						lhs.low_part() * rhs.high_part() +
						a32 * b32,
						a00 * b00);
					result += uint128(a32 * b00) << uint128::int_part_bottom_half_bits;
					result += uint128(a00 * b32) << uint128::int_part_bottom_half_bits;
					return result;
				}
			}
		}

		template<cjm::numerics::concepts::can_find_most_significant_set_bitpos UI128>
		constexpr int most_sign_set_bit(UI128 value) noexcept
		{
			if constexpr (cjm::numerics::concepts::builtin_unsigned_integer<UI128>)
			{
				assert(value != 0);
				auto lz = std::countl_zero(value);
				return static_cast<int>(std::numeric_limits<UI128>::digits - 1 - lz);
			}
			else //has static method
			{
				return UI128::most_sign_set_bit(value);
			}
		}


		constexpr int uint128::most_sign_set_bit(uint128 value) noexcept
		{
			assert(value != 0);
			if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
			{
				if constexpr (constexpr_bit_casting)
				{
					auto lz = internal::countl_zero(bit_cast<natuint128_t>(value));
					return static_cast<int>(std::numeric_limits<natuint128_t>::digits - 1 - lz);
				}
				else
				{
					return uint128::fls(value);
				}
			}
			else
			{
				return uint128::fls(value);
			}
		}

		constexpr uint128::int_part add_with_carry(uint128::int_part lhs, uint128::int_part rhs,
			unsigned char carry_in, unsigned char& carry_out) noexcept
		{
			using int_t = typename uint128::int_part;
			if (std::is_constant_evaluated())
			{
				int_t ret = lhs;
				int_t max = std::max(lhs, rhs);
				if (carry_in)
					++ret;
				ret += rhs;
				carry_out = ret < max ? 1 : 0;
				return ret;
			}
			else
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode ==
					uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					int_t ret = 0;
					carry_out = CJM_ADDCARRY64(carry_in, lhs, rhs, &ret);
					return ret;
				}
				else
				{
					int_t ret = lhs;
					int_t max = std::max(lhs, rhs);
					if (carry_in)
						++ret;
					ret += rhs;
					carry_out = ret < max ? 1 : 0;
					return ret;
				}
			}
		}

		constexpr std::uint64_t add_with_carry_u64(std::uint64_t lhs, std::uint64_t rhs,
			unsigned char carry_in, unsigned char& carry_out) noexcept
		{
			return add_with_carry(lhs, rhs, carry_in, carry_out);
		}


		constexpr std::pair<uint128, unsigned char>
			add_with_carry(uint128 first_addend, uint128 second_addend,
				unsigned char carry_in) noexcept
		{
			if (std::is_constant_evaluated())
			{
				uint128 ret = first_addend;
				if (carry_in)
					++ret;
				ret += second_addend;
				unsigned char carry_out = ret < first_addend;
				return std::make_pair(ret, carry_out);
			}
			else
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					unsigned char carry_1 = 0;
					unsigned char carry_2 = 0;
					auto ret = uint128{};
					ret.m_limbs.m_low = add_with_carry(first_addend.m_limbs.m_low, second_addend.m_limbs.m_low, carry_in, carry_1);
					ret.m_limbs.m_high = add_with_carry(first_addend.m_limbs.m_high, second_addend.m_limbs.m_high, carry_1, carry_2);
					return std::make_pair(ret, carry_2);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					unsigned char carry_out = 0;
					auto temp = cjm::numerics::internal::add_with_carry(cjm::numerics::bit_cast<natuint128_t>(first_addend),
						cjm::numerics::bit_cast<natuint128_t>(second_addend),
						carry_in, carry_out);
					return std::make_pair(temp, carry_out);
				}
				else
				{
					uint128 ret = first_addend;
					if (carry_in)
						++ret;
					ret += second_addend;
					unsigned char carry_out = ret < first_addend;
					return std::make_pair(ret, carry_out);
				}
			}
		}

		constexpr uint128::int_part sub_with_borrow(uint128::int_part minuend, uint128::int_part subtrahend,
			unsigned char borrow_in, unsigned char& borrow_out) noexcept
		{
			using int_t = typename uint128::int_part;
			if (std::is_constant_evaluated())
			{
				int_t ret = minuend;
				if (borrow_in)
				{
					--ret;
				}
				ret -= subtrahend;
				borrow_out = ret > minuend ? 0 : 1;
				return ret;
			}
			else
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					int_t ret = 0;
					borrow_out = CJM_SUBBORROW_64(borrow_in, minuend, subtrahend, &ret);
					return ret;
				}
				else
				{
					int_t ret = minuend;
					if (borrow_in)
					{
						--ret;
					}
					ret -= subtrahend;
					borrow_out = ret > minuend ? 0 : 1;
					return ret;
				}
			}
		}


		constexpr std::pair<uint128, unsigned char> sub_with_borrow(uint128 minuend,
			uint128 subtrahend, unsigned char borrow_in) noexcept
		{
			if (std::is_constant_evaluated())
			{
				uint128 ret = minuend;
				if (borrow_in)
					--ret;
				ret -= subtrahend;
				unsigned char borrow_out = ret > minuend ? 1 : 0;
				return std::make_pair(ret, borrow_out);
			}
			else
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64
					|| calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					unsigned char borrow_1 = 0;
					unsigned char borrow_2 = 0;
					auto ret = uint128{};
					ret.m_limbs.m_low = sub_with_borrow(minuend.m_limbs.m_low,
						subtrahend.m_limbs.m_low, borrow_in, borrow_1);
					ret.m_limbs.m_high = sub_with_borrow(minuend.m_limbs.m_high,
						subtrahend.m_limbs.m_high, borrow_1, borrow_2);
					return std::make_pair(ret, borrow_2);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					unsigned char borrow_out = 0;
					auto temp = cjm::numerics::internal::sub_with_borrow(cjm::numerics::bit_cast<natuint128_t>(minuend),
						cjm::numerics::bit_cast<natuint128_t>(subtrahend),
						borrow_in, borrow_out);
					return std::make_pair(temp, borrow_out);
				}
				else
				{
					uint128 ret = minuend;
					if (borrow_in)
						--ret;
					ret -= subtrahend;
					unsigned char borrow_out = ret > minuend ? 1 : 0;
					return std::make_pair(ret, borrow_out);
				}
			}
		}


		constexpr uint128 operator/(uint128 lhs, uint128 rhs)
		{
			if (std::is_constant_evaluated())
			{
				if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
				uint128 quotient = 0;
				uint128 remainder = 0;
				uint128::constexpr_div_mod_impl(lhs, rhs, &quotient, &remainder);
				return quotient;
			} // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement				
			else // ReSharper disable once CppUnreachableCode
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					return static_cast<natuint128_t>(lhs) / static_cast<natuint128_t>(rhs);
				}
				//restore when figure out problem linking clang unsigned int stuff in ms environment
				/*else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					return bit_cast<uint128>(bit_cast<divonlynatuint128_t>(lhs) / bit_cast<divonlynatuint128_t>(rhs));
				}*/
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)  // NOLINT(readability-misleading-indentation)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::div_mod_msc_x64_impl(lhs, rhs, &quotient, &remainder);
					return quotient;
				}
				else // NOLINT(readability-misleading-indentation)
				{
					// constexpr (calculation_mode == uint128_calc_mode::default_eval)  
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::constexpr_div_mod_impl(lhs, rhs, &quotient, &remainder);
					return quotient;
				}
			}
		}

		constexpr uint128 operator%(uint128 lhs, uint128 rhs)
		{
			if (std::is_constant_evaluated())
			{
				if (rhs == 0)
				{
					throw std::domain_error("Modulus by zero is illegal.");
				}
				uint128 quotient = 0;
				uint128 remainder = 0;
				uint128::constexpr_div_mod_impl(lhs, rhs, &quotient, &remainder);
				return remainder;
			} // ReSharper disable once CppRedundantElseKeywordInsideCompoundStatement			   
			else	 // ReSharper disable once CppUnreachableCode			
			{
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					return static_cast<natuint128_t>(lhs) % static_cast<natuint128_t>(rhs);
				}
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::div_mod_msc_x64_impl(lhs, rhs, &quotient, &remainder);
					return remainder;
				}
				//restore when figure out problem linking clang unsigned int stuff in ms environment
				/*else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
				{
					if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
					return bit_cast<uint128>(bit_cast<divonlynatuint128_t>(lhs) % bit_cast<divonlynatuint128_t>(rhs));
				}*/
				else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
				{
					if (rhs == 0)
					{
						throw std::domain_error("Modulus by zero is illegal.");
					}
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::constexpr_div_mod_impl(lhs, rhs, &quotient, &remainder);
					return remainder;
				}
			}
		}
	}
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::min() noexcept
{
	return 0;
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::lowest() noexcept
{
	return 0;
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::max() noexcept
{
	return cjm::numerics::uint128::make_uint128(
		std::numeric_limits<std::uint64_t>::max(),
		std::numeric_limits<std::uint64_t>::max());
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::epsilon() noexcept
{
	return std::numeric_limits<uint64_t>::epsilon();
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::round_error() noexcept
{
	return std::numeric_limits<uint64_t>::round_error();
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::infinity() noexcept
{
	return std::numeric_limits<uint64_t>::infinity();
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::quiet_NaN() noexcept
{
	return std::numeric_limits<uint64_t>::quiet_NaN();
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::signaling_NaN() noexcept
{
	return std::numeric_limits<uint64_t>::signaling_NaN();
}

constexpr cjm::numerics::uint128 std::numeric_limits<cjm::numerics::uint128>::denorm_min() noexcept
{
	return std::numeric_limits<uint64_t>::denorm_min();
}


template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::array<typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv, 2>
cjm::numerics::u128_parsing_helper<Chars, CharTraits>::
get_hex_tags()
{

	using namespace std::string_view_literals;
	if constexpr (std::is_same_v<char_t, char>)
	{
		std::array<sv, 2> ret{ "0x"sv, "0X"sv };
		return ret;
	}
	else if constexpr (std::is_same_v<char_t, wchar_t>)
	{
		std::array<sv, 2> ret{ L"0x"sv, L"0X"sv };
		return ret;
	}
	else if constexpr (std::is_same_v<char_t, char8_t>)
	{
		std::array<sv, 2> ret{ u8"0x"sv, u8"0X"sv };
		return ret;
	}
	else if constexpr (std::is_same_v<char_t, char16_t>)
	{
		std::array<sv, 2> ret{ u"0x"sv, u"0X"sv };
		return ret;
	}
	else
	{
		std::array<sv, 2> ret{ U"0x"sv, U"0X"sv };
		return ret;
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv cjm::numerics::u128_parsing_helper<Chars,
	CharTraits>::non_decimal_separator()
{
	using namespace std::string_view_literals;

	if constexpr (std::is_same_v<char_t, char>)
	{
		return ",_"sv;
	}
	else if constexpr (std::is_same_v<char_t, wchar_t>)
	{
		return L",_"sv;
	}
	else if constexpr (std::is_same_v<char_t, char8_t>)
	{
		return u8",_"sv;
	}
	else if constexpr (std::is_same_v<char_t, char16_t>)
	{
		return u",_"sv;
	}
	else
	{
		return U",_"sv;
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv
cjm::numerics::u128_parsing_helper<Chars, CharTraits>::decimal_separator()
{
	using namespace std::string_view_literals;
	if constexpr (std::is_same_v<char_t, char>)
	{
		return "."sv;
	}
	else if constexpr (std::is_same_v<char_t, wchar_t>)
	{
		return L"."sv;
	}
	else if constexpr (std::is_same_v<char_t, char8_t>)
	{
		return u8"."sv;
	}
	else if constexpr (std::is_same_v<char_t, char16_t>)
	{
		return u"."sv;
	}
	else
	{
		return U"."sv;
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<Chars, CharTraits>::get_value_hex(char_t c)
{
	using namespace std::string_view_literals;

	if constexpr (std::is_same_v<char_t, char>)
	{
		switch (c)
		{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'A':
		case 'a':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, wchar_t>)
	{
		switch (c)
		{
		case L'0':
			return 0;
		case L'1':
			return 1;
		case L'2':
			return 2;
		case L'3':
			return 3;
		case L'4':
			return 4;
		case L'5':
			return 5;
		case L'6':
			return 6;
		case L'7':
			return 7;
		case L'8':
			return 8;
		case L'9':
			return 9;
		case L'A':
		case L'a':
			return 10;
		case L'b':
		case L'B':
			return 11;
		case L'c':
		case L'C':
			return 12;
		case L'd':
		case L'D':
			return 13;
		case L'e':
		case L'E':
			return 14;
		case L'f':
		case L'F':
			return 15;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, char8_t>)
	{
		switch (c)
		{
		case u8'0':
			return 0;
		case u8'1':
			return 1;
		case u8'2':
			return 2;
		case u8'3':
			return 3;
		case u8'4':
			return 4;
		case u8'5':
			return 5;
		case u8'6':
			return 6;
		case u8'7':
			return 7;
		case u8'8':
			return 8;
		case u8'9':
			return 9;
		case u8'A':
		case u8'a':
			return 10;
		case u8'b':
		case u8'B':
			return 11;
		case u8'c':
		case u8'C':
			return 12;
		case u8'd':
		case u8'D':
			return 13;
		case u8'e':
		case u8'E':
			return 14;
		case u8'f':
		case u8'F':
			return 15;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, char16_t>)
	{
		switch (c)
		{
		case u'0':
			return 0;
		case u'1':
			return 1;
		case u'2':
			return 2;
		case u'3':
			return 3;
		case u'4':
			return 4;
		case u'5':
			return 5;
		case u'6':
			return 6;
		case u'7':
			return 7;
		case u'8':
			return 8;
		case u'9':
			return 9;
		case u'A':
		case u'a':
			return 10;
		case u'b':
		case u'B':
			return 11;
		case u'c':
		case u'C':
			return 12;
		case u'd':
		case u'D':
			return 13;
		case u'e':
		case u'E':
			return 14;
		case u'f':
		case u'F':
			return 15;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
			return 0;
		case U'1':
			return 1;
		case U'2':
			return 2;
		case U'3':
			return 3;
		case U'4':
			return 4;
		case U'5':
			return 5;
		case U'6':
			return 6;
		case U'7':
			return 7;
		case U'8':
			return 8;
		case U'9':
			return 9;
		case U'A':
		case U'a':
			return 10;
		case U'b':
		case U'B':
			return 11;
		case U'c':
		case U'C':
			return 12;
		case U'd':
		case U'D':
			return 13;
		case U'e':
		case U'E':
			return 14;
		case U'f':
		case U'F':
			return 15;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::pair<std::uint8_t, typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv>
cjm::numerics::u128_parsing_helper<Chars, CharTraits>::get_value_hex(sv text)
{
	uint8_t high_nibble, low_nibble;
	auto ret = sv{};
	switch (text.size())
	{
	case 0:
		throw std::invalid_argument{ "Function requires a non-empty string." };
	case 1:
		return std::make_pair(get_value_hex(text[0]), ret);
	case 2:
		high_nibble = get_value_hex(text[0]);
		low_nibble = get_value_hex(text[1]);
		break;
	default:
		high_nibble = get_value_hex(text[0]);
		low_nibble = get_value_hex(text[1]);
		ret = text.substr(2, text.size() - 2);
		break;
	}
	high_nibble <<= (CHAR_BIT / 2);
	high_nibble |= low_nibble;
	return std::make_pair(high_nibble, ret);
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::uint8_t
cjm::numerics::u128_parsing_helper<Chars, CharTraits>::get_value_dec(char_t c)
{
	using namespace std::string_view_literals;

	if constexpr (std::is_same_v<char_t, char>)
	{
		switch (c)
		{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, wchar_t>)
	{
		switch (c)
		{
		case L'0':
			return 0;
		case L'1':
			return 1;
		case L'2':
			return 2;
		case L'3':
			return 3;
		case L'4':
			return 4;
		case L'5':
			return 5;
		case L'6':
			return 6;
		case L'7':
			return 7;
		case L'8':
			return 8;
		case L'9':
			return 9;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, char8_t>)
	{
		switch (c)
		{
		case u8'0':
			return 0;
		case u8'1':
			return 1;
		case u8'2':
			return 2;
		case u8'3':
			return 3;
		case u8'4':
			return 4;
		case u8'5':
			return 5;
		case u8'6':
			return 6;
		case u8'7':
			return 7;
		case u8'8':
			return 8;
		case u8'9':
			return 9;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else if constexpr (std::is_same_v<char_t, char16_t>)
	{
		switch (c)
		{
		case u'0':
			return 0;
		case u'1':
			return 1;
		case u'2':
			return 2;
		case u'3':
			return 3;
		case u'4':
			return 4;
		case u'5':
			return 5;
		case u'6':
			return 6;
		case u'7':
			return 7;
		case u'8':
			return 8;
		case u'9':
			return 9;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
			return 0;
		case U'1':
			return 1;
		case U'2':
			return 2;
		case U'3':
			return 3;
		case U'4':
			return 4;
		case U'5':
			return 5;
		case U'6':
			return 6;
		case U'7':
			return 7;
		case U'8':
			return 8;
		case U'9':
			return 9;
		default:
			throw std::invalid_argument("Unrecognized character");
		}
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr bool cjm::numerics::u128_parsing_helper<Chars, CharTraits>::is_legal_hex_char(char_t c)
{

	if constexpr (std::is_same_v<char_t, char>)
	{
		switch (c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'a':
		case 'b':
		case 'B':
		case 'c':
		case 'C':
		case 'd':
		case 'D':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
			return true;
		default:
			return false;
		}
	}
	else
	{
		switch (static_cast<char>(c))
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'a':
		case 'b':
		case 'B':
		case 'c':
		case 'C':
		case 'd':
		case 'D':
		case 'e':
		case 'E':
		case 'f':
		case 'F':
			return true;
		default:
			return false;
		}
	}
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr  cjm::numerics::uint128 cjm::numerics::u128_parsing_helper<Chars, CharTraits>::
parse_decimal_str(sv decimal_str)
{
	uint128 ret = 0;
	uint128 exp = 1;
	for (auto i = decimal_str.length() - 1; i != std::numeric_limits<size_t>::max(); --i)
	{
		auto oldRet = ret;
		ret += static_cast<uint128>(get_value_dec(decimal_str[i])) * exp;
		if (oldRet > ret)
			throw std::overflow_error("The string represents a number too large to parse as a uint128.");
		exp *= 10;
	}
	return ret;
}


template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr  cjm::numerics::uint128 cjm::numerics::u128_parsing_helper<Chars, CharTraits>::
parse_hex_str(sv hex_str)
{
	constexpr char zero = '0';
	constexpr char_t zero_cast = static_cast<char_t>(zero);
	auto length = hex_str.length();

	if (length < 1)
	{
		throw std::invalid_argument{ "Cannot parse supplied string as 128-bit unsigned integer: string is empty." };
	}
	bool allZero = std::all_of(hex_str.cbegin(), hex_str.cend(), [](char_t c) -> bool
		{
			return c == zero_cast;
		});
	if (hex_str[0] == zero_cast && (length == 1 || allZero))
	{
		return 0;
	}
	if (length == 2)
	{
		uint8_t high_nibble = get_value_hex(hex_str[0]) << (CHAR_BIT / 2);
		uint8_t low_nibble = get_value_hex(hex_str[1]);
		return high_nibble | low_nibble;
	}
	else
	{
		using namespace numerics::uint128_literals;
		const auto first_two = hex_str.substr(0, 2);
		const auto hex_tags = get_hex_tags();
		const bool has_hex_tags = std::any_of(hex_tags.cbegin(), hex_tags.cend(),
			[=](sv tag) -> bool {return tag == first_two; });
		if (has_hex_tags)
		{
			hex_str = hex_str.substr(2, hex_str.size() - 2);
		}
		size_t hex_digits = std::count_if(hex_str.cbegin(), hex_str.cend(),
			[](char_t c) -> bool
			{
				return is_legal_hex_char(c);
			});
		if (hex_digits < 1)
		{
			throw std::invalid_argument{ "Supplied string contains no valid hexadecimal digits." };
		}

		constexpr size_t max_hex_digits = std::numeric_limits<uint128>::digits / (CHAR_BIT / 2); //max of 32 hex digits

		if (hex_digits > max_hex_digits)
		{
			throw std::invalid_argument{ "Supplied string contains too many hex digits to store in uint128." };
		}

		const size_t shift_result_right_amount = (max_hex_digits - hex_digits) * 4u;
		//nb. removed vector and just use array.  not only vector not necessary, but clang can't handle in constexpr context yet
		auto arr = (uint128{}).to_big_endian_arr(); //get an empty zero-filled array
		size_t idx = 0;

		while (!hex_str.empty()) //i.e. more bytes to go
		{
			//get next byte and the remainder of the string not incl byte we just got
			//if no more bytes, new_hex_str will be empty.
			auto [byte, new_hex_str] = get_value_hex(hex_str);
			hex_str = new_hex_str;
			arr[idx++] = byte;
		}
		return (uint128::make_from_bytes_big_endian(arr) >> shift_result_right_amount);
	}
}

namespace cjm::numerics::internal
{
	template<concepts::builtin_floating_point TFloat>
	uint128 make_from_floating_point(TFloat v) noexcept
	{
		using flt_t = std::remove_cvref_t<TFloat>;
		if constexpr (std::is_same_v<flt_t, long double> && !sse3_available &&
			// ReSharper disable once CppRedundantBooleanExpressionArgument
			(compiler == compiler_used::clang || compiler == compiler_used::clang_gcc))
		{
			// Go 50 bits at a time, that fits in a double
			static_assert(std::numeric_limits<double>::digits >= 50, "double digits must >= 50");
			static_assert(std::numeric_limits<long double>::digits <= 150, "long double digits must <= 150.");
			// Undefined behavior if v is not finite or cannot fit into uint128.
			assert(std::isfinite(v) && v > -1 && v < std::ldexp(1.0L, 128));

			v = std::ldexp(v, -100);
			const auto w0 = static_cast<std::uint64_t>(static_cast<double>(std::trunc(v)));
			v = std::ldexp(v - static_cast<double>(w0), 50);
			const auto w1 = static_cast<std::uint64_t>(static_cast<double>(std::trunc(v)));
			v = std::ldexp(v - static_cast<double>(w1), 50);
			const auto w2 = static_cast<std::uint64_t>(static_cast<double>(std::trunc(v)));
			return (static_cast<uint128>(w0) << 100) | (static_cast<uint128>(w1) << 50) |
				static_cast<uint128>(w2);
		}
		else
		{

			// Rounding behavior is towards zero, same as for built-in types.
			// Undefined behavior if v is NaN or cannot fit into uint128.
			assert(std::isfinite(v) && v > -1 &&
				(std::numeric_limits<flt_t>::max_exponent <= 128 ||
					v < std::ldexp(static_cast<flt_t>(1), 128)));

			if (v >= std::ldexp(static_cast<flt_t>(1), 64))
			{
				const auto hi = static_cast<std::uint64_t>(std::ldexp(v, -64));
				const auto lo = static_cast<std::uint64_t>(v - std::ldexp(static_cast<flt_t>(hi), 64));
				return uint128{ hi, lo };
			}
			return static_cast<std::uint64_t>(v);
		}
	}

	template<concepts::builtin_128bit_unsigned_integer Ui128>
	Ui128 add_with_carry(Ui128 first_addend, Ui128 second_addend, unsigned char carry_in,
		unsigned char& carry_out) noexcept
	{
		Ui128 ret = first_addend;
		if (carry_in)
			++ret;
		ret += second_addend;
		carry_out = ret < first_addend ? 1 : 0;
		return ret;
	}

	template<concepts::builtin_128bit_unsigned_integer Ui128>
	Ui128 sub_with_borrow(Ui128 minuend, Ui128 subtrahend, unsigned char borrow_in,
		unsigned char& borrow_out) noexcept
	{
		Ui128 ret = minuend;
		if (borrow_in)
			--ret;
		ret -= subtrahend;
		borrow_out = ret > minuend ? 1 : 0;
		return ret;
	}

	template<concepts::cjm_unsigned_integer Ui128>
	requires (sizeof(Ui128) == 16 && !concepts::builtin_128bit_unsigned_integer<Ui128>)
		Ui128 add_with_carry(Ui128 first_addend, Ui128 second_addend,
			unsigned char carry_in, unsigned char& carry_out) noexcept
	{
		Ui128 ret = first_addend;
		if (carry_in)
			++ret;
		ret += second_addend;
		carry_out = ret < first_addend ? 1 : 0;
		return ret;
	}

	template<concepts::cjm_unsigned_integer Ui128>
	requires (sizeof(Ui128) == 16 && !concepts::builtin_128bit_unsigned_integer<Ui128>)
		Ui128 sub_with_borrow(Ui128 minuend, Ui128 subtrahend,
			unsigned char borrow_in, unsigned char& borrow_out) noexcept
	{
		Ui128 ret = minuend;
		if (borrow_in)
			--ret;
		ret -= subtrahend;
		borrow_out = ret > minuend ? 1 : 0;
		return ret;
	}

}

namespace cjm::numerics
{
	template<concepts::builtin_floating_point TFloat>
	uint128 safe_from_floating_or_throw(TFloat v)
	{
		using flt_t = std::remove_cvref_t<TFloat>;
		if constexpr (std::is_same_v<flt_t, long double> && !sse3_available &&
			// ReSharper disable once CppRedundantBooleanExpressionArgument
			(compiler == compiler_used::clang || compiler == compiler_used::clang_gcc))
		{
			if (!(std::isfinite(v) && v > -1 && v < std::ldexp(1.0L, 128)))
				throw std::invalid_argument{ "Specified floating point value cannot be converted to uint128." };
			return internal::make_from_floating_point(v);
		}
		else
		{
			if (!(std::isfinite(v) && v > -1 &&
				(std::numeric_limits<flt_t>::max_exponent <= 128 ||
					v < std::ldexp(static_cast<flt_t>(1), 128))))
				throw std::invalid_argument{ "Specified floating point value cannot be converted to uint128." };
			return internal::make_from_floating_point(v);
		}
	}

	template<concepts::builtin_floating_point TFloat>
	std::optional<uint128> safe_from_floating(TFloat v) noexcept
	{
		using flt_t = std::remove_cvref_t<TFloat>;
		if constexpr (std::is_same_v<flt_t, long double> && !sse3_available &&
			// ReSharper disable once CppRedundantBooleanExpressionArgument
			(compiler == compiler_used::clang || compiler == compiler_used::clang_gcc))
		{
			if (!(std::isfinite(v) && v > -1 && v < std::ldexp(1.0L, 128)))
				return std::nullopt;
			return std::make_optional(internal::make_from_floating_point(v));
		}
		else
		{
			if (!(std::isfinite(v) && v > -1 &&
				(std::numeric_limits<flt_t>::max_exponent <= 128 ||
					v < std::ldexp(static_cast<flt_t>(1), 128))))
				return std::nullopt;
			return std::make_optional(internal::make_from_floating_point(v));
		}
	}
}

namespace cjm::numerics::internal::consider_delete
{
	using namespace cjm::numerics;
	using namespace uint128_literals;
	using ones_arr = std::array<uint128, 129>;
	constexpr uint128 all_ones = std::numeric_limits<uint128>::max();
	constexpr ones_arr init_ones_lookup() noexcept
	{
		auto ret = ones_arr{};
		uint128 ones = all_ones;
		for (auto i = 128; i > -1; --i)
		{
			ret[i] = all_ones;
			ones <<= 1;
		}
		return ret;
	}
	[[maybe_unused]]
	constexpr inline ones_arr ones_lookup = init_ones_lookup();

}

constexpr cjm::numerics::uint128 GetZero() noexcept
{
	constexpr cjm::numerics::uint128 ret{};
	static_assert(ret == 0);
	return ret;
}

inline void cjm::numerics::uint128::instrumented_div_mod(std::basic_ostream<char>& stream,
	uint128 dividend, uint128 divisor, uint128* quotient_ret, uint128* remainder_ret)
{
	constexpr auto newl = "\n";
	if (divisor == 0)
	{
		throw std::domain_error("Division and/or modulus by zero is forbidden.");
	}
	if (divisor > dividend)
	{
		*quotient_ret = 0;
		*remainder_ret = dividend;
		return;
	}
	if (divisor == dividend)
	{
		*quotient_ret = 1;
		*remainder_ret = 0;
		return;
	}

	stream << std::hex;
	stream << "BEGIN INSTRUMENTED DIV MOD 128" << newl;
	stream << "dividend: 0x" << dividend << newl;
	stream << "divisor: 0x" << divisor << newl;

	auto denominator = divisor;
	auto quotient = make_uint128(0, 0);

	stream << "denominator: 0x" << denominator << newl;
	stream << "quotient: 0x" << quotient << newl;

	const auto flsDividend = fls(dividend);
	const auto flsDenominator = fls(denominator);
	const auto shiftAmount = flsDividend - flsDenominator;
	denominator <<= shiftAmount;
	stream << std::dec;
	stream << "DECIMAL FLS RESULTS:" << newl;
	stream << "flsDividend: " << flsDividend << newl;
	stream << "flsDenominator: " << flsDenominator << newl;
	stream << "shiftAmount: " << shiftAmount << newl;

	stream << std::hex;
	stream << "HEX FLS RESULTS:" << newl;
	stream << "flsDividend: 0x" << flsDividend << newl;
	stream << "flsDenominator: 0x" << flsDenominator << newl;
	stream << "shiftAmount: 0x" << shiftAmount << newl;

	stream << "denominator <<= 0x" << shiftAmount << " == 0x" << denominator << newl;
	stream << std::boolalpha;
	stream << newl << newl << "STARTING CALCULATION LOOP" << newl;
	for (int i = 0; i <= shiftAmount; i++)
	{
		stream << "i = " << std::dec << i << " i <= " << shiftAmount
			<< "= " << (i <= shiftAmount) << std::hex << newl;
		stream << "quotient: 0x" << quotient << newl;
		quotient <<= 1;
		stream << "(quotient <<= 1): 0x" << quotient << newl;
		stream << "denominator: 0x" << denominator << newl;
		bool dividendGreaterThanOrEqualToDenom = dividend >= denominator;
		stream << "dividendGreaterThanOrEqualToDenom: " << dividendGreaterThanOrEqualToDenom << newl;
		if (dividendGreaterThanOrEqualToDenom)
		{
			dividend -= denominator;
			quotient |= 1;
			stream << "dividend -= denominator: 0x" << dividend << newl;
			stream << "quotient |= 1: 0x" << quotient << newl;
		}
		denominator >>= 1;
		stream << "denominator >>= 1: 0x" << denominator << newl;
	}

	stream << "final quotient: 0x" << quotient << newl;
	stream << "final remainder (dividend): 0x" << dividend << newl;
	*quotient_ret = quotient;
	*remainder_ret = dividend;
	stream << "END INSTRUMENTED DIV MOD 128" << newl;
}

inline cjm::numerics::uint128::operator float() const
{
	return static_cast<float>(m_limbs.m_low) + std::ldexp(static_cast<float>(m_limbs.m_high), 64);
}

inline cjm::numerics::uint128::operator double() const
{
	return static_cast<double>(m_limbs.m_low) + std::ldexp(static_cast<double>(m_limbs.m_high), 64);
}

inline cjm::numerics::uint128::operator long double() const
{
	return static_cast<long double>(m_limbs.m_low) +
		std::ldexp(static_cast<long double>(m_limbs.m_high), 64);
}

inline void cjm::numerics::uint128::best_safe_div_mod(uint128 dividend, uint128 divisor,
	uint128* quotient, uint128* remainder)
{
	if (divisor == 0) throw std::domain_error{ "Division and/or modulus by zero is forbidden." };
	if (!quotient) throw std::invalid_argument{ "quotient is nullptr!" };
	if (!remainder) throw std::invalid_argument{ "remainder is nullptr!" };

	uint128::divmod_result_t result = unsafe_div_mod(dividend, divisor);
	*quotient = result.quotient;
	*remainder = result.remainder;
}

//This method is based on the 128-bit unsigned integer division
//provided by clang / LLVM for a built-in unsigned __int128 type
//the original version from which this was based can be found in the
//source repository in the file "Llvm_u128div.txt" The original is
//part of the LLVM project, licensed to CJM Screws, LLC under
//the Apache-2.0 license with LLVM exceptions.  No copyright claimed
//by CJM Screws, LLC to the original, unaltered form of the LLVM code.
//Here, it was modified to use microsoft's intrinsic functions in place
//of GCC/Clang intrinsic functions as well as to use a compiler intrinsic
//for the most optimized path of the algorithm rather than inline assembly
//language (because 1- x64 Microsoft does not support inline assembly and 2-
//because compiler can deal with intrinsics more efficiently than either
//inline or separately written assembly).
//Also, the clang version used unions for type punning which 1-
//(though allowed in C) is undefined behavior in C++ and 2- isn't necessary
//anyway because this function is a static member of uint128 and has
//direct access to its m_high and m_low members anyway.
//If the optimized path from clang cannot be taken, I revert
//to using abseil's shift with subtract algorithm as elsewhere
//rather than using the slow-case handler from clang.
#ifdef CJM_UDIV_INTRINSIC_AVAILABLE
inline void cjm::numerics::uint128::div_mod_msc_x64_impl(uint128 dividend, uint128 divisor,
	uint128* quotient_ret, uint128* remainder_ret) noexcept
{
	constexpr size_t n_utword_bits = sizeof(uint128) * CHAR_BIT;
	assert(quotient_ret != nullptr && remainder_ret != nullptr && divisor != 0);
	uint128 quotient = 0;
	uint128 remainder = 0;
	if (divisor > dividend)
	{
		*remainder_ret = dividend;
		*quotient_ret = 0;
		return;
	}
	// When the divisor fits in 64 bits, we can use an optimized path.
	if (divisor.m_limbs.m_high == 0)
	{
		remainder.m_limbs.m_high = 0;
		if (dividend.m_limbs.m_high < divisor.m_limbs.m_low)
		{
			// The result fits in 64 bits.
			quotient.m_limbs.m_low = CJM_UDIV128(dividend.m_limbs.m_high, dividend.m_limbs.m_low,
				divisor.m_limbs.m_low, &remainder.m_limbs.m_low);
			quotient.m_limbs.m_high = 0;
		}
		else
		{
			// First, divide with the high part to get the remainder in dividend.s.high.
			// After that dividend.s.high < divisor.s.low.
			quotient.m_limbs.m_high = dividend.m_limbs.m_high / divisor.m_limbs.m_low;
			dividend.m_limbs.m_high = dividend.m_limbs.m_high % divisor.m_limbs.m_low;
			quotient.m_limbs.m_low = CJM_UDIV128(dividend.m_limbs.m_high, dividend.m_limbs.m_low,
				divisor.m_limbs.m_low, &remainder.m_limbs.m_low);
		}
		*remainder_ret = remainder;
		*quotient_ret = quotient;
		return;
	}
	unsafe_constexpr_div_mod_impl(dividend, divisor, quotient_ret, remainder_ret);
	return;
}
#else
inline void cjm::numerics::uint128::div_mod_msc_x64_impl([[maybe_unused]] uint128 dividend, [[maybe_unused]] uint128 divisor, uint128* quotient_ret, uint128* remainder_ret) noexcept
{
	//do nothing ... in gcc this never gets called but still needs to be there
	*quotient_ret = 0;
	*remainder_ret = 0;
}
#endif

namespace cjm::numerics::uint128_literals
{
	namespace internal
	{
		template<char... Chars>
		requires (sizeof...(Chars) > 0)
			struct array_retrieval_helper;

		template<char... Chars>
		requires (sizeof...(Chars) > 0)
			struct array_retrieval_helper final
		{
			static CJM_LIT_CONST std::array<char, sizeof...(Chars)> reverse_array();
			static constexpr std::array<char, sizeof...(Chars)> reversed_array_val = reverse_array();
			static constexpr std::array<char, sizeof...(Chars)> array_val{ Chars... };
		};

		template<char... Chars>
		requires (sizeof...(Chars) > 0)
			CJM_LIT_CONST std::array<char, sizeof...(Chars)> array_retrieval_helper<Chars...>::reverse_array()
		{
			std::array<char, sizeof...(Chars)> src{ Chars... };
			std::array<char, sizeof...(Chars)> ret{};
			size_t dst_idx = 0;
			for (size_t src_idx = src.size() - 1; src_idx != std::numeric_limits<size_t>::max(); --src_idx)
			{
				ret[dst_idx++] = src[src_idx];
			}
			return ret;
		}

	}

	template<lit_type LiteralType>
	requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
		CJM_LIT_CONST std::array<std::optional<unsigned short>, 256u> lit_helper::init_digit_lookup()
	{
		if constexpr (LiteralType == lit_type::Decimal)
		{
			return std::array<std::optional<unsigned short>, 256u>{
				std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, 0, 1,
					2, 3, 4, 5, 6,
					7, 8, 9, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt};
		}
		else
		{
			return std::array<std::optional<unsigned short>, 256u>{
				std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, 0, 1,
					2, 3, 4, 5, 6,
					7, 8, 9, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					10, 11, 12, 13, 14,
					15, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, 10, 11, 12,
					13, 14, 15, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
					std::nullopt};
		}
	}
	template<concepts::unsigned_integer Ui>
	CJM_LIT_CONST std::array<char, std::numeric_limits<Ui>::digits10 + 1> lit_helper::get_max_decimal()
	{
		constexpr Ui max = std::numeric_limits<Ui>::max();
		std::array<char, std::numeric_limits<Ui>::digits10 + 1> ret{};
		Ui current = max;
		bool done;
		size_t idx = ret.size() - 1;
		do
		{
			int value = static_cast<int>(current % 10);
			current /= 10;
			ret[idx--] = static_cast<char>(value + 0x30);
			done = idx == std::numeric_limits<size_t>::max();
		} while (!done);
		return ret;
	}

	template<char... Chars>
	CJM_LIT_CONST lit_type lit_helper::get_lit_type()
	{
		std::array<char, sizeof...(Chars)> arr{ Chars... };
		size_t length = arr.size();
		if (length == 0)
		{
			return lit_type::Illegal;
		}
		if (arr[0] == '0' && (length == 1 || are_all_chars_0<Chars...>()))
		{
			return lit_type::Zero;
		}
		if (length < 3)
		{
			if (arr[0] == '0')
				return lit_type::Illegal;
			if (arr[0] < 0x30 || arr[0] > 0x39)
				return lit_type::Illegal;
			return lit_type::Decimal;
		}
		if (arr[0] == '0' && (arr[1] == 'x' || arr[1] == 'X'))
			return lit_helper::is_legal_hex_char(arr[2]) ? lit_type::Hexadecimal : lit_type::Illegal;
		if (arr[0] == '0')
			return lit_type::Illegal;
		return arr[0] >= 0x30 && arr[0] <= 0x39 ? lit_type::Decimal : lit_type::Illegal;
	}

	template<char... Chars>
	CJM_LIT_CONST std::optional<size_t> lit_helper::count_decimal_chars()
	{
		std::array<char, sizeof...(Chars)> arr{ Chars... };
		size_t ret = 0;
		for (char c : arr)
		{
			if (c == '\'') continue;
			int cast = static_cast<int>(c) - 0x30;
			if (cast >= 0 && cast <= 9)
				++ret;
			else
				return std::nullopt;
		}
		return ret;
	}


	template<char... Chars>
	CJM_LIT_CONST lit_type lit_helper::get_chars() noexcept
	{
		constexpr lit_type the_lit_type = get_lit_type<Chars...>();
		return the_lit_type;
	}

	template<char... Chars>
	requires (sizeof...(Chars) > 0)
		CJM_LIT_CONST bool lit_helper::are_all_chars_0()
	{
		std::array<char, sizeof...(Chars)> arr{ Chars... };
		for (char c : arr)
		{
			if (c != '0')
				return false;
		}
		return true;
	}
	CJM_LIT_CONST bool lit_helper::is_legal_hex_char(char c) noexcept
	{
		return lit_helper::digit_lookup_v<lit_type::Hexadecimal>[static_cast<unsigned>(c) % 256u].has_value();
	}

	CJM_LIT_CONST bool lit_helper::is_legal_dec_char(char c) noexcept
	{
		return lit_helper::digit_lookup_v<lit_type::Decimal>[static_cast<unsigned>(c) % 256u].has_value();
	}
	template<concepts::unsigned_integer Ui, char... Chars>
	CJM_LIT_CONST bool lit_helper::validate_decimal_size()
	{
		if (get_lit_type<Chars...>() != lit_type::Decimal)
			return false;

		constexpr size_t max_dec_digits = std::numeric_limits<Ui>::digits10 + 1;
		constexpr std::optional<size_t> dec_digits = count_decimal_chars<Chars...>();
		if (!dec_digits.has_value() || *dec_digits > max_dec_digits)
			return false;
		if (*dec_digits < max_dec_digits)
			return true;
		//the hard part, need if it has max digits
		constexpr auto arr = std::array<char, sizeof...(Chars)>{ Chars... };
		size_t max_idx = 0;
		for (char c : arr)
		{
			if (c == '\'') continue;
			char comparand = max_decimal_digits_v<Ui>[max_idx++];
			if (c < comparand)
				return true;
			if (c > comparand)
				return false;
		}
		return true;
	}

	template<char... Chars>
	CJM_LIT_CONST std::optional<size_t> lit_helper::count_hex_chars()
	{
		if (get_lit_type<Chars...>() != lit_type::Hexadecimal)
			return std::nullopt;
		constexpr auto ok_chars =
			std::array<char, 22u>{
			'0', '1', '2', '3', '4',
				'5', '6', '7', '8', '9',
				'a', 'b', 'c', 'd', 'e',
				'f', 'A', 'B', 'C', 'D',
				'E', 'F' };
		constexpr auto arr = std::array<char, sizeof...(Chars)>{ Chars... };
		if (arr.size() < 3)
			return std::nullopt;
		if (arr[0] != '0' || (arr[1] != 'x' && arr[1] != 'X'))
			return std::nullopt;
		size_t char_count = 0;
		for (size_t idx = 2; idx < arr.size(); ++idx)
		{
			char val = arr[idx];
			if (val == '\'') continue;
			bool found_it = false;
			for (size_t j = 0; j < ok_chars.size(); ++j)
			{
				if (ok_chars[j] == val)
				{
					found_it = true;
					break;
				}
			}
			if (found_it)
				++char_count;
			else
				return std::nullopt;
		}
		return char_count;
	}

	template<concepts::unsigned_integer Ui, size_t Digits, lit_type LiteralType, char... Chars>
	requires (sizeof...(Chars) > 0 && sizeof...(Chars) >= Digits && Digits > 0 &&
		(LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal))
		CJM_LIT_CONST std::optional<Ui> lit_helper::execute_literal_parse()
	{
		using reverser_t = typename internal::array_retrieval_helper<Chars...>;
		Ui ret = 0;
		constexpr Ui base_factor = LiteralType == lit_type::Decimal ? 10 : 16;
		constexpr auto& arr = digit_lookup_v<LiteralType>;
		Ui current_digit = 1;
		size_t processed_digits = 0;
		size_t digit_idx = 0;
		while (processed_digits < Digits && digit_idx < reverser_t::reversed_array_val.size())
		{
			char digit = reverser_t::reversed_array_val[digit_idx++];
			if (digit == '\'') continue;
			std::optional<unsigned short> value = arr[static_cast<size_t>(digit)];
			if (value.has_value())
			{
				ret += (*value * current_digit);
				current_digit *= base_factor;
				++processed_digits;
			}
			else
			{
				return std::nullopt;
			}
		}
		if (processed_digits == Digits)
			return ret;
		else
			return std::nullopt;
	}

	template <concepts::unsigned_integer Ui, char... Chars>
	CJM_LIT_CONST std::optional<Ui> lit_helper::parse_literal()
	{
		constexpr lit_type type = get_lit_type<Chars...>();
		if (type != lit_type::Decimal && type != lit_type::Hexadecimal && type != lit_type::Zero)
			return std::nullopt;
		if constexpr (type == lit_type::Zero)
		{
			return 0;
		}
		else
		{
			using reverser_t = typename internal::array_retrieval_helper<Chars...>;
			if constexpr (type == lit_type::Decimal)
			{
				if constexpr (!validate_decimal_size<Ui, Chars...>())
				{
					return std::nullopt;
				}
				else
				{
					constexpr std::optional<size_t> decimal_size = count_decimal_chars<Chars...>();
					if constexpr (decimal_size.has_value())
					{
						static_assert(*decimal_size <= reverser_t::array_val.size());
						constexpr auto reversed_array = internal::array_retrieval_helper<Chars...>::reversed_array_val;
						if constexpr (reversed_array.size() > 0)
							return execute_literal_parse<Ui, *decimal_size, lit_type::Decimal, Chars...>();
						else
							return std::nullopt;
					}
					else
					{
						return std::nullopt;
					}
				}
			}
			else //hexadecimal
			{
				constexpr std::optional<size_t> hex_chars = count_hex_chars<Chars...>();
				if constexpr (hex_chars.has_value())
				{
					constexpr size_t num_digits = *hex_chars;
					if constexpr (num_digits <= max_hex_digits_v<Ui>)
					{
						return execute_literal_parse<Ui, num_digits, lit_type::Hexadecimal, Chars...>();
					}
					else
					{
						return std::nullopt;
					}
				}
				else
				{
					return std::nullopt;
				}
			}
		}
	}
}
#endif
