#ifndef CJM_UINT128_INL
#define CJM_UINT128_INL
#include "uint128.hpp"
#include <algorithm>
#include "cjm_string.hpp"
#include <cwctype>


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
//
// -----------------------------------------------------------------------------
// File: int128.h 
// -----------------------------------------------------------------------------


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
            constexpr uint128 operator"" _u128()
            {
                auto charArray = uint128_lit_helper::get_array<Chars...>();
                return uint128_lit_helper::parse_from_char_array(charArray);
            }

            constexpr uint8_t uint128_lit_helper::get_hex_value(char c)
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
                    case 'a':
                        return 10;
                    case 'b':
                        return 11;
                    case 'c':
                        return 12;
                    case 'd':
                        return 13;
                    case 'e':
                        return 14;
                    case 'f':
                        return 15;
                    default:
                        throw std::invalid_argument("Unrecognized character");
                }
            }

            template<size_t Size>
            constexpr std::pair<bool, size_t> uint128_lit_helper::scan_chars_dec(std::array<char, Size> arr)
            {
                std::pair<bool, size_t> ret;
                if (arr.empty())
                {
                    ret.first = false;
                    ret.second = 0;
                }
                else
                {
                    char current_char = arr[0];
                    if (current_char == '0' || !(current_char >= 0x30 && current_char <= 0x39))
                    {
                        ret.first = false;
                        ret.second = 0;
                    }
                    else
                    {
                        ret.first = true;
                        ret.second = 1;
                        current_char = ret.second < arr.size() ? arr[ret.second] : '\0';
                        if (current_char != '\0')
                        {
                            while (current_char != '\0')
                            {
                                ++ret.second;
                                if (ret.first)
                                    ret.first = (current_char == '\'') || (current_char >= 0x30 && current_char <= 0x39);
                                current_char = ret.second < arr.size() ? arr[ret.second] : '\0';
                            }
                        }
                        else
                        {
                            ret.second = 1;
                        }
                    }
                }
                return ret;
            }

            template<size_t Size>
            constexpr std::pair<size_t, size_t> uint128_lit_helper::get_dec_val(std::array<char, Size> arr, size_t index)
            {
                //if we got here we know that every char in chars is either \' or a legal digit
                std::pair<size_t, size_t> ret;
                char current_char = arr[index--];
                if (current_char == '\'')
                {
                    //since we cannot reasonably expect a literal to ever have size_t::max digits,
                    //if it equals the size_t::max, that means it was zero (zero - 1 == max) that
                    //in turn means that the first character was a separator, which is not legal.
                    if (index == std::numeric_limits<size_t>::max())
                    {
                        throw std::domain_error("Cannot begin with a separator.");
                    }
                    while (current_char == '\'')
                    {
                        if (index == 0 && arr[0] == '\'')
                        {
                            throw std::domain_error("Cannot begin with a separator.");
                        }
                        current_char = arr[index--];
                    }
                    ret.first = static_cast<size_t>(current_char) & 0x000Full;
                    assert(ret.first <= 9);
                    ret.second = index;
                }
                else
                {
                    ret.first = static_cast<size_t>(current_char) & 0x000Full;
                    ret.second = index;
                }
                return ret;
            }

            constexpr char uint128_lit_helper::to_lower(char c) noexcept
            {
                return (c >= 65 && c <= 90) ?
                       static_cast<char>(static_cast<unsigned char>(c + 0x20)) :
                       c;
            }

            template<size_t Size>
            constexpr uint128 uint128_lit_helper::get_hex_literal(std::array<char, Size> arr)
            {
                size_t length = arr.size();
                if (length < 3)
                    throw std::domain_error("Bad literal");
                if (arr[0] != '0' || (arr[1] != 'X' && arr[1] != 'x'))
                    throw std::domain_error("Only hexadecimal literals are allowed.");

                uint128 value = 0;
                size_t byteCount = 0;
                for (size_t i = length - 1; i > 1; )
                {
                    if (byteCount >= (sizeof(uint128)))
                    {
                        throw std::domain_error("The literal is too long.");
                    }
                    auto pair = get_byte(arr, i);
                    uint128 insertMe = static_cast<uint128>(pair.first) << (static_cast<int>(byteCount++) * CHAR_BIT);
                    value |= insertMe;
                    i = pair.second;
                }
                return value;
            }
            template<size_t Size>
            constexpr uint128 uint128_lit_helper::get_decimal_literal(std::array<char, Size> arr)
            {
                uint128 ret = 0;
                auto result = scan_chars_dec(arr);
                if (result.first)
                {
                    uint128 exponent = 1;
                    size_t length = result.second;
                    for (auto i = length - 1; i != std::numeric_limits<size_t>::max(); )
                    {
                        auto temp = get_dec_val(arr, i);
                        uint128 retCopy = ret;
                        ret += (temp.first * exponent);
                        if (ret < retCopy)
                        {
                            throw std::domain_error("literal too large to fit.");
                        }
                        exponent *= 10;
                        i = temp.second;
                    }
                    return ret;
                }
                throw std::domain_error("Bad decimal literal");
            }

            template <char... Chars>
            constexpr std::array<char, sizeof...(Chars)> uint128_lit_helper::get_array()
            {
                std::array<char, sizeof...(Chars)> ret{ Chars... };
                return ret;
            }

            template<size_t Size>
            constexpr lit_type uint128_lit_helper::get_lit_type(std::array<char, Size> arr)
            {
                size_t length = arr.size();
                if (length == 0)
                {
                    return lit_type::Illegal;
                }
                if (arr[0] == '0' && (length == 1 || are_all_chars_0(arr)))
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
                    return is_legal_hex_char(arr[2]) ? lit_type::Hexadecimal : lit_type::Illegal;
                if (arr[0] == '0')
                    return lit_type::Illegal;
                return arr[0] >= 0x30 && arr[0] <= 0x39 ? lit_type::Decimal : lit_type::Illegal;
            }

            constexpr bool uint128_lit_helper::is_legal_hex_char(char c) noexcept
            {
                c = to_lower(c);
                return ((c >= 0x30 && c <= 0x39) || (c >= 0x61 && c <= 0x66));
            }

            template<size_t Size>
            constexpr bool uint128_lit_helper::are_all_chars_0(std::array<char, Size> arr)
            {
                for (size_t i = 0; i < arr.size(); i++)
                {
                    if (arr[i] != '0' && arr[i] != '\'')
                        return false;
                }
                return true;
            }

            template<size_t Size>
            constexpr std::pair<std::uint8_t, size_t> uint128_lit_helper::get_byte(std::array<char, Size> arr, size_t index)
            {

                char currentChar = to_lower(arr[index]);
                while (currentChar == 0 || currentChar == '\'')
                {
                    currentChar = to_lower(arr[--index]);
                }
                --index;
                uint8_t bottomValue = get_hex_value(currentChar);
                if (index < 2)
                {
                    return std::make_pair(bottomValue, index);
                }
                currentChar = to_lower(arr[index]);
                while (currentChar == 0 || currentChar == '\'')
                {
                    --index;
                    if (index < 2)
                    {
                        return std::make_pair(bottomValue, index);
                    }
                    currentChar = to_lower(arr[index]);
                }
                uint8_t topValue = get_hex_value(currentChar);
                topValue <<= 4;
                topValue |= bottomValue;
                return std::make_pair(topValue, --index);
            }

            template <size_t Size>
            constexpr uint128 uint128_lit_helper::parse_from_char_array(std::array<char, Size> arr)
            {
                lit_type lt = get_lit_type(arr);
                uint128 temp;
                switch (lt)
                {
                    default:  // NOLINT(clang-diagnostic-covered-switch-default) 
                    case lit_type::Illegal:
                        throw std::domain_error("Illegal literal: only hexadecimal and decimal are supported.");
                    case lit_type::Decimal:
                        temp = get_decimal_literal(arr);
                        break;
                    case lit_type::Hexadecimal:
                        temp = get_hex_literal(arr);
                        break;
                    case lit_type::Zero:
                        temp = 0;
                        break;
                }
                return temp;
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
			if (static_cast<common_type_t>(width) > static_cast<common_type_t>( rep.size()))
			{
				iosflags adjustfield = flags & ios::adjustfield;
				if (adjustfield == ios::left)
				{
					rep.append(static_cast<str_size_t>(static_cast<common_type_t>(width) - static_cast<common_type_t>(rep.size())), os.fill());
				}
				else if (adjustfield == ios::internal && (flags & ios::showbase) && (flags & ios::basefield) == ios::hex && v != 0)
				{
					width = static_cast<common_type_t>(rep.size());
					rep.insert(2, static_cast<str_size_t>(width), os.fill());
				}
				else
				{
					width = static_cast<common_type_t>(rep.size());
					rep.insert(0, static_cast<str_size_t>(width), os.fill());
				}
			}
			return os << rep;
		}

		

		template<typename Char, typename CharTraits, typename Allocator>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
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
			uint128 low=0;
			best_safe_div_mod(high, div, &high, &low);
			uint128 mid=0;
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
				if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
				{
					return divmod_result_t{ static_cast<natuint128_t>(dividend) / static_cast<natuint128_t>(divisor), static_cast<natuint128_t>(dividend) % static_cast<natuint128_t>(divisor) };
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

#ifdef CJM_NUMERICS_LITTLE_ENDIAN
		constexpr uint128::uint128() noexcept  : m_low{}, m_high{} {}
#else
		constexpr uint128::uint128() noexcept : m_high{}, m_low{} {}
#endif

		constexpr uint128::divmod_result_t uint128::div_mod(uint128 dividend, uint128 divisor)
		{
			if (divisor == 0) throw std::domain_error("Division and/or modulus by zero is forbidden.");
			return unsafe_div_mod(dividend, divisor);
		}

		constexpr uint128::uint128(int v) noexcept
			: m_low{ static_cast<int_part>(v) },
			m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
		constexpr uint128::uint128(unsigned int v) noexcept : m_low{ v }, m_high{ 0 } {}
		constexpr uint128::uint128(long v) noexcept
			: m_low{ static_cast<int_part>(v) },
			m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
		constexpr uint128::uint128(unsigned long v) noexcept
			: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}
		constexpr uint128::uint128(long long v) noexcept
			: m_low{ static_cast<int_part>(v) },
			m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
		constexpr uint128::uint128(unsigned long long v) noexcept
			: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}

		constexpr uint128& uint128::operator=(int v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(unsigned int v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(long v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(unsigned long v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(long long v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
			return *this;
		}
		constexpr uint128& uint128::operator=(unsigned long long v) noexcept
		{
			m_low = static_cast<int_part>(v);
			m_high = 0;
			return *this;
		}

        template<typename Chars, typename CharTraits>
            requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
		uint128 uint128::make_from_string(std::basic_string_view<Chars, CharTraits> parseMe)
		{
			using ph = u128_parsing_helper<Chars, CharTraits>;
			auto str = std::basic_string<Chars, CharTraits>{parseMe};
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
			return m_low || m_high;
		}
		constexpr uint128::operator char() const noexcept
		{
			return static_cast<char>(m_low);
		}
		constexpr uint128::operator signed char() const noexcept
		{
			return static_cast<signed char>(m_low);
		}
		constexpr uint128::operator unsigned char() const noexcept
		{
			return static_cast<unsigned char>(m_low);
		}

        constexpr uint128::operator char8_t() const noexcept
        {
            return static_cast<char8_t>(m_low);
        }

        constexpr uint128::operator char16_t() const noexcept
		{
			return static_cast<char16_t>(m_low);
		}
		constexpr uint128::operator char32_t() const noexcept
		{
			return static_cast<char32_t>(m_low);
		}
		constexpr uint128::operator wchar_t() const noexcept
		{
			return static_cast<wchar_t>(m_low);
		}
		constexpr uint128::operator short() const noexcept
		{
			return static_cast<short>(m_low);
		}
		constexpr uint128::operator unsigned short() const noexcept
		{
			return static_cast<unsigned short>(m_low);
		}
		constexpr uint128::operator int() const noexcept
		{
			return static_cast<int>(m_low);
		}
		constexpr uint128::operator unsigned int() const noexcept
		{
			return static_cast<unsigned int>(m_low);
		}
		constexpr uint128::operator long() const noexcept
		{
			return static_cast<long>(m_low);
		}
		constexpr uint128::operator unsigned long() const noexcept
		{
			return static_cast<unsigned long>(m_low);
		}
		constexpr uint128::operator long long() const noexcept
		{
			return static_cast<long long>(m_low);
		}
		constexpr uint128::operator unsigned long long() const noexcept
		{
			return static_cast<unsigned long long>(m_low);
		}

		constexpr size_t uint128::hash_code() const noexcept
		{
			return calculate_hash(m_high, m_low);
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
			*this = *this << amount;
			return *this;
		}

		constexpr uint128& uint128::operator>>=(int amount) noexcept
		{
			*this = *this >> amount;
			return *this;
		}

		constexpr uint128& uint128::operator&=(uint128 other) noexcept
		{
			m_high &= other.m_high;
			m_low &= other.m_low;
			return *this;
		}

		constexpr uint128& uint128::operator|=(uint128 other) noexcept
		{
			m_high |= other.m_high;
			m_low |= other.m_low;
			return *this;
		}

		constexpr uint128& uint128::operator^=(uint128 other) noexcept
		{
			m_high ^= other.m_high;
			m_low ^= other.m_low;
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
			return m_low;
		}

		constexpr uint128::int_part uint128::high_part() const noexcept
		{
			return m_high;
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
			: m_low(low), m_high(high) {}

		constexpr size_t uint128::calculate_hash(int_part hi, int_part low) noexcept
		{
			static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4, "Only 32 and 64 bit architecture supported.");
			size_t hash{ 0 };
			if constexpr(sizeof(size_t) == 8)
			{
#pragma warning(push)
#pragma warning (disable: 4244) //this warning pops in in msvc when compiling as 32 bit even though this if constexpr branch never taken in that case.
				hash = low + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash = hi + 0x9e3779b9 + (hash << 6) + (hash >> 2);
#pragma warning(pop)				
			}
			else // ReSharper disable once CppUnreachableCode
			{
				size_t hi_hi = hi >> 32;
				size_t hi_low = static_cast<size_t>(hi & 0x0000'FFFF);
				size_t low_hi = low >> 32;
				size_t low_low = static_cast<size_t>(low & 0x0000'FFFF);

				hash = low_low + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash = low_hi + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash = hi_low + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash = hi_hi + 0x9e3779b9 + (hash << 6) + (hash >> 2);
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
					for (size_t index = 0 ; index < byteCount; ++index)
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
						auto all_ones_in_byte = 0xff_u128;
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
						auto all_ones_in_byte = 0xff00'0000'0000'0000'0000'0000'0000'0000_u128;
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
				ret.m_high = shift_me.m_low;
				ret.m_low = 0;
				ret.m_high =  (ret.m_high << (shift_amount - 64)); 
			}
			else
			{
				ret.m_high = CJM_LSHIFT128(shift_me.m_low, shift_me.m_high, static_cast<unsigned char>(shift_amount));
				ret.m_low = shift_me.m_low << shift_amount;
			}
			return ret;			
		}

		inline uint128 uint128::rshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept
		{
			assert(shift_amount > -1 && shift_amount < std::numeric_limits<uint128>::digits);
			auto ret = uint128{};
			if (shift_amount >= 64)
			{
				ret.m_high = 0;
				ret.m_low = shift_me.m_high;
				ret.m_low = ret.m_low >> (shift_amount - 64);
			}
			else
			{
				ret.m_low = CJM_RSHIFT128(shift_me.m_low, shift_me.m_high, static_cast<unsigned char>(shift_amount));
				ret.m_high = shift_me.m_high >> shift_amount;
			}
			return ret;
		}

		constexpr void uint128::constexpr_div_mod_impl(uint128 dividend, uint128 divisor, uint128* quotient_ret,
		                                     uint128* remainder_ret)
		{
           
			if (divisor == 0)
			{
				throw std::domain_error("Division and/or modulus by zero is forbidden.");
			}
			unsafe_constexpr_div_mod_impl(dividend, divisor, quotient_ret, remainder_ret);			
		}

		constexpr void uint128::unsafe_constexpr_div_mod_impl(uint128 dividend, uint128 divisor, uint128* quotient_ret,
			uint128* remainder_ret) noexcept
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

        constexpr int internal::countl_zero(uint128 ui) noexcept
        {
            assert(ui != 0);
            auto hp = ui.high_part();
        	if ( hp != 0)
        	{
                return std::countl_zero(hp);
        	}
            return std::numeric_limits<typename uint128::int_part>::digits + std::countl_zero(ui.low_part());
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
			else
			{
				if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
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
			return lhs == rhs ? std::strong_ordering::equal : ((lhs > rhs) ? std::strong_ordering::greater : std::strong_ordering::less);
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
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return -static_cast<natuint128_t>(operand);
                }
//		        else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
//      	    {
//
//      	    }
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
                assert(amount < std::numeric_limits<uint128>::digits && amount > -1);
                // uint64_t shifts of >= 64 are undefined, so we will need some
                // special-casing.
                auto absAmount = static_cast<int>(math_functions::int_abs(amount));
                if (absAmount < static_cast<int>(uint128::int_part_bits))
                {
                    if (absAmount != 0)
                    {
                        return uint128::make_uint128((lhs.high_part() >> absAmount),
                                                    (lhs.low_part() >> absAmount) |
                                                    (lhs.high_part() << (static_cast<int>(uint128::int_part_bits) -absAmount)));
                    }
                    return lhs;
                }
                return uint128::make_uint128(0, lhs.high_part() >> (absAmount - static_cast<int>(uint128::int_part_bits)));
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) >> amount;
                }
		        else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
      			{
					return uint128::rshift_msvc_x64(lhs, amount);
      			}
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
                {
                    //It is undefined behavior to attempt a shift greater than number of bits in an integral type
                    assert(amount < std::numeric_limits<uint128>::digits && amount > -1);
                    // uint64_t shifts of >= 64 are undefined, so we will need some
                    // special-casing.
                    auto absAmount = static_cast<int>(math_functions::int_abs(amount));
                    if (absAmount < static_cast<int>(uint128::int_part_bits))
                    {
                        if (absAmount != 0)
                        {
                            return uint128::make_uint128((lhs.high_part() >> absAmount),
                                                        (lhs.low_part() >> absAmount) |
                                                        (lhs.high_part() << (static_cast<int>(uint128::int_part_bits) -absAmount)));
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
                assert(amount < std::numeric_limits<uint128>::digits && amount > -1);
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
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return bit_cast<natuint128_t>(lhs) << amount;
                }
		        else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
      			{
					assert(amount < std::numeric_limits<uint128>::digits && amount > -1);
					return uint128::lshift_msvc_x64(lhs, amount);
            	}
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
                {
                    //It is undefined behavior to attempt a shift greater than number of bits in an integral type
                    assert(amount < std::numeric_limits<uint128>::digits && amount > -1);
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
            }

		}

		constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits && static_cast<int>(amount) > -1);
			return (lhs >> static_cast<int>(amount));
		}
		constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits && static_cast<int>(amount) > -1);
			return (lhs << static_cast<int>(amount));
		}
		//arithmetic operators
		constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept
		{
            if (std::is_constant_evaluated())
            {
                uint128 result = uint128::make_uint128(lhs.high_part() + rhs.high_part(),
                                                      lhs.low_part() + rhs.low_part());
                if (result.low_part() < lhs.low_part()) // check for carry
                {
                    return uint128::make_uint128(result.high_part() + 1, result.low_part());
                }
                return result;
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) + static_cast<natuint128_t>(rhs);
                }
//	            else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
//              {
//
//              }
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
                {
                    uint128 result = uint128::make_uint128(lhs.high_part() + rhs.high_part(),
                                                          lhs.low_part() + rhs.low_part());
                    if (result.low_part() < lhs.low_part()) // check for carry
                    {
                        return uint128::make_uint128(result.high_part() + 1, result.low_part());
                    }
                    return result;
                }
            }

		}
		constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept
		{
            if (std::is_constant_evaluated())
            {
                uint128 result = uint128::make_uint128(lhs.high_part() - rhs.high_part(),
                                                      lhs.low_part() - rhs.low_part());
                if (lhs.low_part() < rhs.low_part()) // check for borrow
                {
                    return uint128::make_uint128(result.high_part() - 1, result.low_part());
                }
                return result;
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) - static_cast<natuint128_t>(rhs);
                }
//	            else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
//              {
//
//              }
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
                {
                    uint128 result = uint128::make_uint128(lhs.high_part() - rhs.high_part(),
                                                          lhs.low_part() - rhs.low_part());
                    if (lhs.low_part() < rhs.low_part()) // check for borrow
                    {
                        return uint128::make_uint128(result.high_part() - 1, result.low_part());
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
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) * static_cast<natuint128_t>(rhs);
                }
	            else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
	            {
					std::uint64_t carry = 0;
					std::uint64_t low_product = CJM_UMUL128(lhs.low_part(), rhs.low_part(), &carry);
					return uint128::make_uint128(lhs.low_part() * rhs.high_part() + lhs.high_part() * rhs.low_part() + carry, low_product);
		        }
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
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

        constexpr uint128 operator/(uint128 lhs, uint128 rhs)
		{
		    if (std::is_constant_evaluated())
		    {
                if (rhs == 0) { throw std::domain_error("Division by zero is illegal."); }
                uint128 quotient = 0;
                uint128 remainder = 0;
                uint128::constexpr_div_mod_impl(lhs, rhs, &quotient, &remainder);
                return quotient;
            }
		    else
            {
		        if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) / static_cast<natuint128_t>(rhs);
                }
		        else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
				{
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::div_mod_msc_x64_impl(lhs, rhs, &quotient, &remainder);
					return quotient;
				}
                else // constexpr (calculation_mode == uint128_calc_mode::default_eval)
                {
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
            }
            else
            {
                if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
                {
                    return static_cast<natuint128_t>(lhs) % static_cast<natuint128_t>(rhs);
                }
				else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
				{
					uint128 quotient = 0;
					uint128 remainder = 0;
					uint128::div_mod_msc_x64_impl(lhs, rhs, &quotient, &remainder);
					return remainder;
				}
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

#pragma warning( push )
#pragma warning ( disable : 4100)
template<>
// ReSharper disable once CppParameterNeverUsed
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::int_sign([[maybe_unused]] uint128 val) noexcept
{
	return uint128{ 1 };
}
#pragma warning ( pop )
template<>
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::int_gcd(uint128 first, //NOLINT (bugprone-exception-escape)
    uint128 second) noexcept //modulus operator only throws for zero divisor.  logic herein prevents divisor from being zero
{
    assert(first != 0 || second != 0);
	while (second != 0)
	{
		uint128 r = first % second;
		first = second;
		second = r;
	}
	return first;
}
template<>
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::int_lcm(uint128 first, uint128 second) 
{
	if (first == 0 && second == 0)
	{
		return 0;
	}
	if (first == 0)
	{
		std::swap(first, second);
	}
	uint128 product = first * second;
	if (product == 0)
		return 0;
	if (product < first || product < second)
		throw std::overflow_error("Integer overflow occurred.");
	return product / (int_gcd(first, second));
}

template <>
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::floor_log2(uint128 val)
{
	if (val < 1)
		throw std::domain_error("Illegal attempt to find base-2 logarithm of zero or a negative number.");
	int rshiftCount = 0;
	while (val > 1)
	{
		val >>= 1;
		++rshiftCount;
	}
	return rshiftCount;
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::array<typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv, 2>
    cjm::numerics::u128_parsing_helper<Chars, CharTraits>::
        get_hex_tags()
{
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
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
                                                                                                         CharTraits>::
non_decimal_separator()
{
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
    using namespace std::string_view_literals;

    if constexpr (std::is_same_v<char_t, char>)
    {
        return ","sv;
    }
    else if constexpr (std::is_same_v<char_t, wchar_t>)
    {
        return L","sv;
    }
    else if constexpr (std::is_same_v<char_t, char8_t>)
    {
        return u8","sv;
    }
    else if constexpr (std::is_same_v<char_t, char16_t>)
    {
        return u","sv;
    }
    else
    {
        return U","sv;
    }
}

template<typename Chars, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv
    cjm::numerics::u128_parsing_helper<Chars, CharTraits>::decimal_separator()
{
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
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
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
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

template<typename Chars, typename CharTraits = std::char_traits<Chars>>
requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
constexpr std::pair<std::uint8_t, typename cjm::numerics::u128_parsing_helper<Chars, CharTraits>::sv> cjm::numerics::u128_parsing_helper<Chars, CharTraits>::get_value_hex(sv text)
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
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<Chars, CharTraits>::get_value_dec(char_t c)
{
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
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
    static_assert(  std::is_same_v<char_t, char> ||
                    std::is_same_v<char_t, wchar_t> ||
                    std::is_same_v<char_t, char8_t> ||
                    std::is_same_v<char_t, char16_t> ||
                    std::is_same_v<char_t, char32_t>, "It needs to be a character.");
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
        switch(static_cast<char>(c))
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
    };
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
        const bool has_hex_tags = std::any_of(hex_tags.cbegin(), hex_tags.cend(), [=](sv tag) -> bool {return tag == first_two; });
		if (has_hex_tags)
		{
            hex_str = hex_str.substr(2, hex_str.size() - 2);            
		}
        auto hex_digits = std::count_if(hex_str.cbegin(), hex_str.cend(), [](char_t c) -> bool
            {
                return is_legal_hex_char(c);
            });
        if (hex_digits < 1)
        {
            throw std::invalid_argument{ "Supplied string contains no valid hexadecimal digits." };
        }
        constexpr auto max_hex_digits = std::numeric_limits<uint128>::digits / (CHAR_BIT / 2); //max of 32 hex digits
        if (hex_digits > max_hex_digits)
        {
            throw std::invalid_argument{ "Supplied string contains too many hex digits to store in uint128." };
        }
        
        std::vector<uint8_t> temp;
        temp.reserve(std::numeric_limits<uint128>::digits / (CHAR_BIT)); //reserve room for 16 bytes
        while (!hex_str.empty()) //i.e. more bytes to go
        {
            //get next byte and the remainder of the string not incl byte we just got
        	//if no more bytes, new_hex_str will be empty.
            auto [byte, new_hex_str] = get_value_hex(hex_str); 
            hex_str = new_hex_str;
            temp.push_back(byte);
        }
		auto arr = (0_u128).to_big_endian_arr();
        std::copy(temp.cbegin(), temp.cend(), arr.begin());
        return uint128::make_from_bytes_big_endian(arr);
	}   
}


/*
 * Evil macro'd up version of fls64 from google:
 * 		// Returns the 0-based position of the last set bit (i.e., most significant bit)
		// in the given uint64_t. The argument may not be 0.
		//
		// For example:
		//   Given: 5 (decimal) == 101 (binary)
		//   Returns: 2
#define STEP(T, n, pos, sh)           \
		  do {                                        \
			if ((n) >= (static_cast<T>(1) << (sh))) { \
			  (n) = (n) >> (sh);                      \
			  (pos) |= (sh);                          \
			}                                         \
		  } while (0)
		constexpr int uint128::fls64_v1(std::uint64_t n)
		{
			assert(n != 0);
			int pos = 0;
			STEP(uint64_t, n, pos, 0x20);
			auto n32 = static_cast<std::uint32_t>(n);
			STEP(std::uint32_t, n32, pos, 0x10);
			STEP(std::uint32_t, n32, pos, 0x08);
			STEP(std::uint32_t, n32, pos, 0x04);
			return pos + ((std::uint64_t{ 0x3333333322221100 } >> (n32 << 2)) & 0x3);
		}
#undef STEP
 */
#endif
