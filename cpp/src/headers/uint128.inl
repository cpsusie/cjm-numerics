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

namespace cjm
{
	namespace numerics
	{

		template <typename Char, typename CharTraits, typename Allocator>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
		std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v)
		{
			using ios = std::basic_ios<Char, CharTraits>;
			using iosflags = typename ios::fmtflags;
			iosflags flags = os.flags();
			auto rep = uint128::to_string<Char, CharTraits, Allocator>(v, flags);
			std::streamsize width = os.width(0);
			if (static_cast<size_t>(width) > rep.size())
			{
				iosflags adjustfield = flags & ios::adjustfield;
				if (adjustfield == ios::left)
				{
					rep.append(width - rep.size(), os.fill());
				}
				else if (adjustfield == ios::internal && (flags & ios::showbase) && (flags & ios::basefield) == ios::hex && v != 0)
				{
					rep.insert(2, width = rep.size(), os.fill());
				}
				else
				{
					rep.insert(0, width = rep.size(), os.fill());
				}
			}
			return os << rep;
		}

		

		template<typename Char, typename CharTraits, typename Allocator>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
		std::basic_string<Char, CharTraits, Allocator> uint128::to_string(uint128 item, std::ios_base::fmtflags flags)
		{
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
			uint128 low;
			div_mod_impl(high, div, &high, &low);
			uint128 mid;
			div_mod_impl(high, div, &high, &mid);
			if (high.low_part() != 0)
			{
				os << high.low_part();
				os << std::noshowbase << std::setfill('0') << std::setw(div_base_log);
				os << mid.low_part();
				os << std::setw(div_base_log);
			}
			else if (mid.low_part() != 0)
			{
				os << mid.low_part();
				os << std::noshowbase << std::setfill('0') << std::setw(div_base_log);
			}
			os << low.low_part();
			return os.str();
		}

		constexpr uint128 uint128::make_from_bytes_little_endian(byte_array bytes) noexcept
		{
			using ret_t = uint128;
			constexpr size_t byteCount = sizeof(ret_t);
			static_assert(byteCount == bytes.size());
			constexpr size_t byte_shift_size = CHAR_BIT;
			using byte = unsigned char;
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

		constexpr uint128 uint128::MakeUint128(std::uint64_t high, std::uint64_t low) noexcept
		{
			return uint128(high, low);
		}

		constexpr uint128::uint128(int v) noexcept
			: m_low{ static_cast<int_part>(v) },
			m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
		constexpr uint128::uint128(unsigned v) noexcept : m_low{ v }, m_high{ 0 } {}
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
		constexpr uint128& uint128::operator=(unsigned v) noexcept
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
		template <typename Chars, typename CharTraits, typename Allocator>
		uint128 uint128::make_from_string(std::basic_string<Chars, CharTraits, Allocator> parseMe)
		{
			using ph = u128_parsing_helper<Chars, CharTraits>;
			auto trimmed = ph::trim_and_strip(parseMe);
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
				throw std::exception();
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
		constexpr uint128::operator unsigned() const noexcept
		{
			return static_cast<unsigned>(m_low);
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

		constexpr uint128::byte_array uint128::to_little_endian_arr() const noexcept
		{
			using byte = unsigned char;
			byte_array ret{};
			constexpr size_t shiftAmount = CHAR_BIT;
			for (size_t bytesToShift = 0; bytesToShift < ret.size(); ++bytesToShift)
			{
				auto temp = (*this >> static_cast<int>((bytesToShift * shiftAmount)));
				auto b = static_cast<byte>(temp);
				ret[bytesToShift] = b;
			}
			return ret;
		}

		constexpr uint128::uint128(int_part high, int_part low) noexcept
			: m_low(low), m_high(high) {}

		constexpr size_t uint128::calculate_hash(int_part hi, int_part low) noexcept
		{
			static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4, "Only 32 and 64 bit architecture supported.");
			size_t hash{ 0 };
			if constexpr(sizeof(size_t) == 8)
			{
				hash = low + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash = hi + 0x9e3779b9 + (hash << 6) + (hash >> 2);
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

		constexpr void uint128::div_mod_impl(uint128 dividend, uint128 divisor, uint128* quotient_ret,
			uint128* remainder_ret)
		{
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

		constexpr int uint128::fls(uint128 n)
		{
			int_part hi = n.high_part();
			if (hi != 0)
			{
				auto flsHigh = fls_int_part(hi);
				auto sum = flsHigh + int_part_bits;
				return sum;
			}
			return fls_int_part(n.low_part());
		}

		template <typename T>
		constexpr void uint128::step(T& n, int& pos, int shift)
		{
			if (n >= (static_cast<T>(1) << shift))
			{
				n = n >> shift;
				pos = pos | shift;
			}
		}

		constexpr int uint128::fls_int_part(std::uint64_t n)
		{
			assert(n != 0);
			int pos = 0;
			step<std::uint64_t>(n, pos, 0x20);
			auto n32 = static_cast<std::uint32_t>(n);
			step<std::uint32_t>(n32, pos, 0x10);
			step<std::uint32_t>(n32, pos, 0x08);
			step<std::uint32_t>(n32, pos, 0x04);
			return pos + ((std::uint64_t{ 0x3333333322221100 } >> (n32 << 2)) & 0x3);
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
			using intpart = uint128::int_part;
			intpart high = ~operand.high_part();
			intpart lo = ~operand.low_part() + 1;
			if (lo == 0)
				++high; // carry
			return uint128::MakeUint128(high, lo);
		}
		constexpr uint128 operator+(uint128 operand) noexcept
		{
			return operand;
		}
		constexpr uint128 operator~(uint128 operand) noexcept
		{
			return uint128::MakeUint128(~operand.high_part(), ~operand.low_part());
		}
		constexpr uint128 operator!(uint128 operand) noexcept
		{
			return !operand.high_part() && !operand.low_part();
		}
		//Logical operators
		constexpr uint128 operator&(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::MakeUint128(lhs.high_part() & rhs.high_part(),
				lhs.low_part() & rhs.low_part());
		}
		constexpr uint128 operator|(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::MakeUint128(lhs.high_part() | rhs.high_part(),
				lhs.low_part() | rhs.low_part());
		}
		constexpr uint128 operator^(uint128 lhs, uint128 rhs) noexcept
		{
			return uint128::MakeUint128(lhs.high_part() ^ rhs.high_part(),
				lhs.low_part() ^ rhs.low_part());
		}
		//bit shift operators
		constexpr uint128 operator>>(uint128 lhs, int amount) noexcept
		{
			//It is undefined behavior to attempt a shift greater than number of bits in an integral type
			assert(amount < std::numeric_limits<uint128>::digits);
			// uint64_t shifts of >= 64 are undefined, so we will need some
			// special-casing.
			auto absAmount = math_functions::int_abs(amount);
			if (absAmount < uint128::int_part_bits)
			{
				if (absAmount != 0)
				{
					return uint128::MakeUint128((lhs.high_part() >> absAmount),
						(lhs.low_part() >> absAmount) |
						(lhs.high_part() << (uint128::int_part_bits -absAmount)));
				}
				return lhs;
			}
			return uint128::MakeUint128(0, lhs.high_part() >> (absAmount - uint128::int_part_bits));
		}
		constexpr uint128 operator<<(uint128 lhs, int amount) noexcept
		{
			//It is undefined behavior to attempt a shift greater than number of bits in an integral type
			assert(amount < std::numeric_limits<uint128>::digits);
			// uint64_t shifts of >= 64 are undefined, so we will need some
			// special-casing.
			auto absAmount = math_functions::int_abs(amount);
			if (absAmount < uint128::int_part_bits)
			{
				if (absAmount != 0)
				{
					return uint128::MakeUint128((lhs.high_part() << absAmount) |
						(lhs.low_part() >> (uint128::int_part_bits - absAmount)), lhs.low_part() << absAmount);
				}
				return lhs;
			}
			return uint128::MakeUint128(lhs.low_part() << (absAmount - uint128::int_part_bits), 0);
		}

		constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			auto absAmount = static_cast<uint128::int_part>(amount);
			if (absAmount < uint128::int_part_bits)
			{
				if (absAmount != 0)
				{
					return uint128::MakeUint128((lhs.high_part() >> absAmount),
						(lhs.low_part() >> absAmount) |
						(lhs.high_part() << (uint128::int_part_bits - absAmount)));
				}
				return lhs;
			}
			return uint128::MakeUint128(0, lhs.high_part() >> (absAmount - uint128::int_part_bits));
		}
		constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept
		{
			assert(amount < std::numeric_limits<uint128>::digits);
			auto absAmount = static_cast<uint128::int_part>(amount);
			if (absAmount < uint128::int_part_bits)
			{
				if (absAmount != 0)
				{
					return uint128::MakeUint128((lhs.high_part() << absAmount) |
						(lhs.low_part() >> (uint128::int_part_bits - absAmount)), lhs.low_part() << absAmount);
				}
				return lhs;
			}
			return uint128::MakeUint128(lhs.low_part() << (absAmount - uint128::int_part_bits), 0);
		}
		//arithmetic operators
		constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept
		{
			uint128 result = uint128::MakeUint128(lhs.high_part() + rhs.high_part(),
				lhs.low_part() + rhs.low_part());
			if (result.low_part() < lhs.low_part()) // check for carry
			{
				return uint128::MakeUint128(result.high_part() + 1, result.low_part());
			}
			return result;
		}
		constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept
		{
			uint128 result = uint128::MakeUint128(lhs.high_part() - rhs.high_part(),
				lhs.low_part() - rhs.low_part());
			if (lhs.low_part() < rhs.low_part()) // check for borrow
			{
				return uint128::MakeUint128(result.high_part() - 1, result.low_part());
			}
			return result;
		}
		constexpr uint128 operator*(uint128 lhs, uint128 rhs) noexcept
		{
			using int_part = uint128::int_part;
			int_part a32 = lhs.low_part() >> uint128::int_part_bottom_half_bits;
			int_part a00 = lhs.low_part() & uint128::int_part_bottom_half_bitmask;
			int_part b32 = rhs.low_part() >> uint128::int_part_bottom_half_bits;
			int_part b00 = rhs.low_part() & uint128::int_part_bottom_half_bitmask;

			uint128 result = uint128::MakeUint128(lhs.high_part() * rhs.low_part() +
				lhs.low_part() * rhs.high_part() +
				a32 * b32,
				a00 * b00);
			result += uint128(a32 * b00) << uint128::int_part_bottom_half_bits;
			result += uint128(a00 * b32) << uint128::int_part_bottom_half_bits;
			return result;
		}
		constexpr uint128 operator/(uint128 lhs, uint128 rhs)
		{
			if (rhs == 0)
			{
				throw std::domain_error("Division by zero is illegal.");
			}
			uint128 quotient = 0;
			uint128 remainder = 0;
			uint128::div_mod_impl(lhs, rhs, &quotient, &remainder);
			return quotient;
		}

		constexpr uint128 operator%(uint128 lhs, uint128 rhs)
		{
			if (rhs == 0)
			{
				throw std::domain_error("Modulus by zero is illegal.");
			}
			uint128 quotient = 0;
			uint128 remainder = 0;
			uint128::div_mod_impl(lhs, rhs, &quotient, &remainder);
			return remainder;
		}




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
				if (arr.size() == 0)
				{
					ret.first = false;
					ret.second = 0;
				}
				else
				{
					char currentChar = arr[0];
					if (currentChar == '0' || !(currentChar >= 0x30 && currentChar <= 0x39))
					{
						ret.first = false;
						ret.second = 0;
					}
					else
					{
						ret.first = true;
						ret.second = 1;
						currentChar = ret.second < arr.size() ? arr[ret.second] : '\0';
						if (currentChar != '\0')
						{
							while (currentChar != '\0')
							{
								++ret.second;
								if (ret.first)
									ret.first = (currentChar == '\'') || (currentChar >= 0x30 && currentChar <= 0x39);
								currentChar = ret.second < arr.size() ? arr[ret.second] : '\0';
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
				char currentChar = arr[index--];
				if (currentChar == '\'')
				{
					//since we cannot reasonably expect a literal to ever have size_t::max digits,
					//if it equals the size_t::max, that means it was zero (zero - 1 == max) that
					//in turn means that the first character was a separator, which is not legal.
					if (index == std::numeric_limits<size_t>::max())
					{
						throw std::domain_error("Cannot begin with a separator.");
					}
					while (currentChar == '\'')
					{
						if (index == 0 && arr[0] == '\'')
						{
							throw std::domain_error("Cannot begin with a separator.");
						}
						currentChar = arr[index--];
					}
					ret.first = static_cast<size_t>(currentChar) & 0x000Full;
					assert(ret.first <= 9);
					ret.second = index;
				}
				else
				{
					ret.first = static_cast<size_t>(currentChar) & 0x000Full;
					ret.second = index;
				}
				return ret;				
			}

			constexpr char uint128_lit_helper::to_lower(char c) noexcept
			{
				return (c >= 65 && c <= 90) ?
					c + 0x20 :
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
				default:
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
	return cjm::numerics::uint128::MakeUint128(
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
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::int_sign(uint128 val) noexcept
{
	return uint128{ 1 };
}
#pragma warning ( pop )
template<>
constexpr cjm::numerics::uint128 cjm::numerics::math_functions::int_gcd(uint128 first, uint128 second) noexcept
{
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

template <typename CharTraits>
constexpr std::array<typename cjm::numerics::u128_parsing_helper<char, CharTraits>::sv, 2> cjm::numerics::u128_parsing_helper<char, CharTraits>::
get_hex_tags()
{
	std::array<sv, 2> ret{ "0x", "0X" };
	return ret;
}

template <typename CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<char, CharTraits>::sv cjm::numerics::u128_parsing_helper<char,
                                                                                                         CharTraits>::
non_decimal_separator()
{
	return ",";
}

template <typename CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<char, CharTraits>::sv cjm::numerics::u128_parsing_helper<char,
                                                                                                         CharTraits>::
decimal_separator()
{
	return ".";
}

template <typename CharTraits>
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<char, CharTraits>::get_value_hex(char c)
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

template <typename CharTraits>
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<char, CharTraits>::get_value_dec(char c)
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
template <typename CharTraits>
template <typename Allocator>
std::basic_string<char, CharTraits, Allocator> cjm::numerics::u128_parsing_helper<char, CharTraits>::trim_and_strip(
	str128<Allocator> trim_and_stripme)
{
	auto trimmed = string::trim(std::move(trim_and_stripme));
	trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), non_decimal_separator()[0]), trimmed.end());
	return trimmed;
}

template <typename CharTraits>
template <typename Allocator>
std::basic_string<wchar_t, CharTraits, Allocator> cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::
trim_and_strip(str128<Allocator> trim_and_stripme)
{
	auto trimmed = string::trim(std::move(trim_and_stripme));
	trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), non_decimal_separator()[0]), trimmed.end());
	return trimmed;
}

template <typename CharTraits>
template <typename Allocator>
constexpr cjm::numerics::u128_str_format cjm::numerics::u128_parsing_helper<char, CharTraits>::get_format(
	const str128<Allocator>& string)
{
	auto length = string.length();
	if (length < 1)
		return u128_str_format::Illegal;
	bool allZero = std::all_of(string.cbegin(), string.cend(), [] (char c) -> bool
	{
		return c == '0';
	});
	if (string[0] == '0' && (length == 1 || allZero))
		return u128_str_format::Zero;
	if (length < 3)
	{
		if (string[0] == '0')
			return u128_str_format::Illegal;
		return std::all_of(string.cbegin(), string.cend(), 
			[](char c) -> bool { return c >= 0x30 && c <= 0x39; }) ? 
				u128_str_format::Decimal : 
				u128_str_format::Illegal;
	}
	sv firstTwo = string.substr(0, 2);
	auto hex_tags = get_hex_tags();
	bool hasHexTag = std::any_of(hex_tags.cbegin(), hex_tags.cend(), [=](sv tag) -> bool { return tag == firstTwo; });
	if (!hasHexTag)
	{
		return string[0] != '0' && std::all_of(string.cbegin(), string.cend(), 
			[](char c) -> bool { return c >= 0x30 && c <= 0x39; }) 
				?	u128_str_format::Decimal : 
					u128_str_format::Illegal;
	}
	sv afterFirstTwo = string.substr(2, string.length() - 2);
	return std::all_of(afterFirstTwo.cbegin(), afterFirstTwo.cend(), [](char c) -> bool {return is_legal_hex_char(c); }) 
		?	u128_str_format::Hexadecimal : 
			u128_str_format::Illegal;
}

template <typename CharTraits>
constexpr bool cjm::numerics::u128_parsing_helper<char, CharTraits>::is_legal_hex_char(char c)
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

template <typename CharTraits>
constexpr cjm::numerics::uint128 cjm::numerics::u128_parsing_helper<char, CharTraits>::
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

template <typename CharTraits>
constexpr std::array<typename cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::sv, 2> cjm::numerics::
u128_parsing_helper<wchar_t, CharTraits>::get_hex_tags()
{
	std::array<sv, 2> ret{ L"0x", L"0X" };
	return ret;
}

template <typename CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::sv cjm::numerics::u128_parsing_helper<
wchar_t, CharTraits>::non_decimal_separator()
{
	return L",";
}

template <typename CharTraits>
constexpr typename cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::sv cjm::numerics::u128_parsing_helper<
wchar_t, CharTraits>::decimal_separator()
{
	return L".";
}

template <typename CharTraits>
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::get_value_hex(wchar_t c)
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

template <typename CharTraits>
constexpr std::uint8_t cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::get_value_dec(wchar_t c)
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

template <typename CharTraits>
template <typename Allocator>
constexpr cjm::numerics::u128_str_format cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::get_format(
	const str128<Allocator>& string)
{
	auto length = string.length();
	if (length < 1)
		return u128_str_format::Illegal;
	bool allZero = std::all_of(string.cbegin(), string.cend(), [](wchar_t c) -> bool
	{
		return c == L'0';
	});
	if (string[0] == L'0' && (length == 1 || allZero))
		return u128_str_format::Zero;
	if (length < 3)
	{
		if (string[0] == L'0')
			return u128_str_format::Illegal;
		return std::all_of(string.cbegin(), string.cend(),
			[](wchar_t c) -> bool { return std::iswdigit(c); }) ?
			u128_str_format::Decimal :
			u128_str_format::Illegal;
	}
	sv firstTwo = string.substr(0, 2);
	auto hex_tags = get_hex_tags();
	bool hasHexTag = std::any_of(hex_tags.cbegin(), hex_tags.cend(), [=](sv tag) -> bool { return tag == firstTwo; });
	if (!hasHexTag)
	{
		return string[0] != L'0' && std::all_of(string.cbegin(), string.cend(),
			[](wchar_t c) -> bool { return std::iswdigit(c); })
			? u128_str_format::Decimal :
			u128_str_format::Illegal;
	}
	sv afterFirstTwo = string.substr(2, string.length() - 2);
	return std::all_of(afterFirstTwo.cbegin(), afterFirstTwo.cend(), [](wchar_t c) -> bool {return is_legal_hex_wchar_t(c); })
		? u128_str_format::Hexadecimal :
		u128_str_format::Illegal;
}

template <typename CharTraits>
constexpr bool cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::is_legal_hex_wchar_t(wchar_t c)
{
	switch (c)
	{
	case L'0':
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
	case L'A':
	case L'a':
	case L'b':
	case L'B':
	case L'c':
	case L'C':
	case L'd':
	case L'D':
	case L'e':
	case L'E':
	case L'f':
	case L'F':
		return true;
	default:
		return false;
	}
}

template <typename CharTraits>
constexpr cjm::numerics::uint128 cjm::numerics::u128_parsing_helper<wchar_t, CharTraits>::
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
