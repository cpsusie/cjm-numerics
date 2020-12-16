#ifndef CJM_FIXED_UINT_INL
#define CJM_FIXED_UINT_INL
#include "fixed_uint.hpp"
#include "streams.hpp"

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator-(const fixed_uint<LimbType>& operand) noexcept
{
	using int_part = typename fixed_uint<LimbType>::int_part;
	int_part high = ~operand.high_part();
	int_part lo = ~operand.low_part() + 1;
	if (lo == 0)
		++high; // carry
	return fixed_uint<LimbType>::make_fixed_uint(high, lo);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator+(const fixed_uint<LimbType>& operand) noexcept
{
	return operand;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator~(const fixed_uint<LimbType>& operand) noexcept
{
	return fixed_uint<LimbType>::make_fixed_uint(~operand.high_part(), ~operand.low_part());
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator!(const fixed_uint<LimbType>& operand) noexcept
{
	return !operand.high_part() && !operand.low_part();
}


template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator>>(fixed_uint<LimbType> lhs, int amount) 
noexcept
{
	return (lhs >>= amount);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator
<<(fixed_uint<LimbType> lhs, int amount) noexcept
{
	return (lhs <<= amount);
}

template <typename LimbType, typename TUInt>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator>>
(fixed_uint<LimbType> lhs, TUInt amount) noexcept
{
	static_assert(std::is_constructible_v<TUInt, uintmax_t> && std::numeric_limits<TUInt>::is_integer && !std::numeric_limits<TUInt>::is_signed,
		"TUInt must be an unsigned integer for which there exists a conversion to uintmax_t.");
	return (lhs >>= static_cast<std::uintmax_t>(amount));
}

template <typename LimbType, typename TUInt>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::operator<<
(fixed_uint<LimbType> lhs, TUInt amount) noexcept
{
	static_assert(std::is_constructible_v<TUInt, uintmax_t> && std::numeric_limits<TUInt>::is_integer && !std::numeric_limits<TUInt>::is_signed,
		"TUInt must be an unsigned integer for which there exists a conversion to uintmax_t.");
	return (lhs <<= static_cast<std::uintmax_t>(amount));
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr Char cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::zero_char()
{
	static_assert(std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t> ||
		std::is_same_v<Char, char16_t> || std::is_same_v<Char, char32_t>,
		"Character type not supported.");
	Char ret{};
	if constexpr (std::is_same_v<Char, char>)
		ret = '0';
	else if constexpr (std::is_same_v<wchar_t, Char>)
		ret = L'0';
	else if constexpr (std::is_same_v<char16_t, Char>)
		ret = u'0';
	else
		ret = U'0';
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr typename cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::sv cjm::numerics::
fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::non_decimal_separator()
{
	sv ret;
	if constexpr (std::is_same_v<Char, char>)
		ret = ",_'";
	else if constexpr (std::is_same_v<wchar_t, Char>)
		ret = L",_'";
	else if constexpr (std::is_same_v<char16_t, Char>)
		ret = u",_'";
	else
		ret = U",_'";
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr typename cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::sv cjm::numerics::
fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::decimal_separator()
{
	sv ret;
	if constexpr (std::is_same_v<Char, char>)
		ret = ".";
	else if constexpr (std::is_same_v<wchar_t, Char>)
		ret = L".";
	else if constexpr (std::is_same_v<char16_t, Char>)
		ret = u".";
	else
		ret = U".";
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr std::uint8_t cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
get_hex_value(Char c)
{
	uint8_t ret;
	if constexpr (std::is_same_v<Char, char>)
	{
		switch (c)
		{
		case '0':
			ret = 0;
			break;
		case '1':
			ret = 1;
			break;
		case '2':
			ret = 2;
			break;
		case '3':
			ret = 3;
			break;
		case '4':
			ret = 4;
			break;
		case '5':
			ret = 5;
			break;
		case '6':
			ret = 6;
			break;
		case '7':
			ret = 7;
			break;
		case '8':
			ret = 8;
			break;
		case '9':
			ret = 9;
			break;
		case 'a':
		case 'A':
			ret = 10;
			break;
		case 'b':
		case 'B':
			ret = 11;
			break;
		case 'c':
		case 'C':
			ret = 12;
			break;
		case 'd':
		case 'D':
			ret = 13;
			break;
		case 'e':
		case 'E':
			ret = 14;
			break;
		case 'f':
		case 'F':
			ret = 15;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	else if constexpr (std::is_same_v<wchar_t, Char>)
	{
		switch (c)
		{
		case L'0':
			ret = 0;
			break;
		case L'1':
			ret = 1;
			break;
		case L'2':
			ret = 2;
			break;
		case L'3':
			ret = 3;
			break;
		case L'4':
			ret = 4;
			break;
		case L'5':
			ret = 5;
			break;
		case L'6':
			ret = 6;
			break;
		case L'7':
			ret = 7;
			break;
		case L'8':
			ret = 8;
			break;
		case L'9':
			ret = 9;
			break;
		case L'a':
		case L'A':
			ret = 10;
			break;
		case L'b':
		case L'B':
			ret = 11;
			break;
		case L'c':
		case L'C':
			ret = 12;
			break;
		case L'd':
		case L'D':
			ret = 13;
			break;
		case L'e':
		case L'E':
			ret = 14;
			break;
		case L'f':
		case L'F':
			ret = 15;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	else if constexpr (std::is_same_v<char16_t, Char>)
	{
		switch (c)
		{
		case u'0':
			ret = 0;
			break;
		case u'1':
			ret = 1;
			break;
		case u'2':
			ret = 2;
			break;
		case u'3':
			ret = 3;
			break;
		case u'4':
			ret = 4;
			break;
		case u'5':
			ret = 5;
			break;
		case u'6':
			ret = 6;
			break;
		case u'7':
			ret = 7;
			break;
		case u'8':
			ret = 8;
			break;
		case u'9':
			ret = 9;
			break;
		case u'a':
		case u'A':
			ret = 10;
			break;
		case u'b':
		case u'B':
			ret = 11;
			break;
		case u'c':
		case u'C':
			ret = 12;
			break;
		case u'd':
		case u'D':
			ret = 13;
			break;
		case u'e':
		case u'E':
			ret = 14;
			break;
		case u'f':
		case u'F':
			ret = 15;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
			ret = 0;
			break;
		case U'1':
			ret = 1;
			break;
		case U'2':
			ret = 2;
			break;
		case U'3':
			ret = 3;
			break;
		case U'4':
			ret = 4;
			break;
		case U'5':
			ret = 5;
			break;
		case U'6':
			ret = 6;
			break;
		case U'7':
			ret = 7;
			break;
		case U'8':
			ret = 8;
			break;
		case U'9':
			ret = 9;
			break;
		case U'a':
		case U'A':
			ret = 10;
			break;
		case U'b':
		case U'B':
			ret = 11;
			break;
		case U'c':
		case U'C':
			ret = 12;
			break;
		case U'd':
		case U'D':
			ret = 13;
			break;
		case U'e':
		case U'E':
			ret = 14;
			break;
		case U'f':
		case U'F':
			ret = 15;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr std::uint8_t cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
get_decimal_value(Char c)
{
	uint8_t ret;
	if constexpr (std::is_same_v<Char, char>)
	{
		switch (c)
		{
		case '0':
			ret = 0;
			break;
		case '1':
			ret = 1;
			break;
		case '2':
			ret = 2;
			break;
		case '3':
			ret = 3;
			break;
		case '4':
			ret = 4;
			break;
		case '5':
			ret = 5;
			break;
		case '6':
			ret = 6;
			break;
		case '7':
			ret = 7;
			break;
		case '8':
			ret = 8;
			break;
		case '9':
			ret = 9;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal decimal character.");
		}
	}
	else if constexpr (std::is_same_v<wchar_t, Char>)
	{
		switch (c)
		{
		case L'0':
			ret = 0;
			break;
		case L'1':
			ret = 1;
			break;
		case L'2':
			ret = 2;
			break;
		case L'3':
			ret = 3;
			break;
		case L'4':
			ret = 4;
			break;
		case L'5':
			ret = 5;
			break;
		case L'6':
			ret = 6;
			break;
		case L'7':
			ret = 7;
			break;
		case L'8':
			ret = 8;
			break;
		case L'9':
			ret = 9;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	else if constexpr (std::is_same_v<char16_t, Char>)
	{
		switch (c)
		{
		case u'0':
			ret = 0;
			break;
		case u'1':
			ret = 1;
			break;
		case u'2':
			ret = 2;
			break;
		case u'3':
			ret = 3;
			break;
		case u'4':
			ret = 4;
			break;
		case u'5':
			ret = 5;
			break;
		case u'6':
			ret = 6;
			break;
		case u'7':
			ret = 7;
			break;
		case u'8':
			ret = 8;
			break;
		case u'9':
			ret = 9;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
			ret = 0;
			break;
		case U'1':
			ret = 1;
			break;
		case U'2':
			ret = 2;
			break;
		case U'3':
			ret = 3;
			break;
		case U'4':
			ret = 4;
			break;
		case U'5':
			ret = 5;
			break;
		case U'6':
			ret = 6;
			break;
		case U'7':
			ret = 7;
			break;
		case U'8':
			ret = 8;
			break;
		case U'9':
			ret = 9;
			break;
		default:
			throw std::invalid_argument("Supplied character is not a legal hex character.");
		}
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr cjm::numerics::fixed_uint_format cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
get_format(const sv& string)
{
	static constexpr auto zero = zero_char();
	static constexpr hextag_array hexTags = get_hex_tags();
	auto length = string.length();
	if (length < 1)
		return fixed_uint_format::Illegal;
	bool allZero = std::all_of(string.cbegin(), string.cend(), [] (Char c) -> bool
	{
		return c == zero;
	});

	auto isCharLegalDec = [](Char c) -> bool {return is_legal_dec_char(c); };
	if (string[0] == '0' && (length == 1 || allZero))
		return fixed_uint_format::Zero;
	if (length < 3)
	{
		if (string[0] == zero)
			return fixed_uint_format::Illegal;
		return std::all_of(string.cbegin(), string.cend(), 
			isCharLegalDec) ? 
				fixed_uint_format::Decimal : fixed_uint_format::Illegal;
	}
	sv firstTwo = string.substr(0, 2);
	bool hasHexTag = std::any_of(hexTags.cbegin(), hexTags.cend(), 
		[=](sv tag) -> bool {return tag == firstTwo; });
	if (!hasHexTag)
	{
		return string[0] != zero && 
			std::all_of(string.cbegin(), string.cend(), is_legal_dec_char) 
			?		fixed_uint_format::Decimal 
				:	fixed_uint_format::Illegal;
	}
	sv afterFirstTwo = string.substr(2, string.length() - 2);
	return std::all_of(afterFirstTwo.cbegin(), afterFirstTwo.cend(), 
		is_legal_hex_char) 
			? 
				fixed_uint_format::Hexadecimal : 
				fixed_uint_format::Illegal;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr bool cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::is_legal_hex_char(Char c)
{
	bool ret;
	if constexpr (std::is_same_v<Char, char>)
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
		case 'a':
		case 'A':
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
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else if constexpr (std::is_same_v<wchar_t, Char>)
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
		case L'a':
		case L'A':
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
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else if constexpr (std::is_same_v<char16_t, Char>)
	{
		switch (c)
		{
		case u'0':
		case u'1':
		case u'2':
		case u'3':
		case u'4':
		case u'5':
		case u'6':
		case u'7':
		case u'8':
		case u'9':
		case u'a':
		case u'A':
		case u'b':
		case u'B':
		case u'c':
		case u'C':
		case u'd':
		case u'D':
		case u'e':
		case u'E':
		case u'f':
		case u'F':
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
		case U'1':
		case U'2':
		case U'3':
		case U'4':
		case U'5':
		case U'6':
		case U'7':
		case U'8':
		case U'9':
		case U'a':
		case U'A':
		case U'b':
		case U'B':
		case U'c':
		case U'C':
		case U'd':
		case U'D':
		case U'e':
		case U'E':
		case U'f':
		case U'F':
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr bool cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::is_legal_dec_char(Char c)
{
	bool ret;
	if constexpr (std::is_same_v<Char, char>)
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
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else if constexpr (std::is_same_v<wchar_t, Char>)
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
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else if constexpr (std::is_same_v<char16_t, Char>)
	{
		switch (c)
		{
		case u'0':
		case u'1':
		case u'2':
		case u'3':
		case u'4':
		case u'5':
		case u'6':
		case u'7':
		case u'8':
		case u'9':
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	else
	{
		switch (c)
		{
		case U'0':
		case U'1':
		case U'2':
		case U'3':
		case U'4':
		case U'5':
		case U'6':
		case U'7':
		case U'8':
		case U'9':
			ret = true;
			break;
		default:
			ret = false;
			break;
		}
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr size_t cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
legal_hex_char_count(const sv& string)
{
	return std::count_if(string.cbegin(), string.cend(), 
		[](Char c) -> bool {return is_legal_hex_char(c); });
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
constexpr typename cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::hextag_array cjm::numerics
::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::get_hex_tags()
{
	hextag_array ret{};
	if constexpr (std::is_same_v<Char, char>)
	{		
		ret[0] = "0x";
		ret[1] = "0X";
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		ret[0] = L"0x";
		ret[1] = L"0X";
	}
	else if constexpr (std::is_same_v<Char, char16_t>)
	{
		ret[0] = u"0x";
		ret[1] = u"0X";
	}
	else
	{
		ret[0] = U"0x";
		ret[1] = U"0X";
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
UnsignedInteger cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
parse_decimal_string(const sv& decimalString)
{
	UnsignedInteger ret = 0;
	UnsignedInteger exp = 1;
	for (size_t i = decimalString.length() - 1; i != std::numeric_limits<size_t>::max(); --i)
	{
		auto oldRet = ret;
		ret += static_cast<UnsignedInteger>(get_decimal_value(decimalString[i])) * exp;
		if (oldRet > ret)
			throw std::overflow_error("The string represents a number too large to store in an unsigned integer of the type specified.");
		exp *= 10;
	}
	return ret;

}

template <typename UnsignedInteger, typename Char, typename CharTraits>
UnsignedInteger cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::
parse_hex_string(const sv& hexString)
{
	UnsignedInteger ret = 0;
	size_t shiftPlaces = 0;
	constexpr static size_t shiftIncrement = CHAR_BIT /2;
	sv hexTagsRemoved = hexString.substr(2, hexString.length() - 2);
	assert(hexTagsRemoved.length() > 0);
	UnsignedInteger temp = 0;
	size_t hexCharCount = legal_hex_char_count(hexTagsRemoved);
	if (hexCharCount > max_hex_chars)
		throw std::overflow_error("The string represents a number too large to store in an unsigned integer of the type specified.");
	for (size_t i = hexTagsRemoved.length() - 1; i != std::numeric_limits<size_t>::max(); --i)
	{
		temp = static_cast<UnsignedInteger>(get_hex_value(hexTagsRemoved[i]));
		temp <<= shiftPlaces;
		ret |= temp;
		shiftPlaces += shiftIncrement;
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
template <typename Allocator>
UnsignedInteger cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::parse_decimal_string(
	fixed_uint_str<Allocator> parseMe)
{
	sv converted = static_cast<sv>(parseMe);
	return parse_decimal_string(converted);
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
template <typename Allocator>
UnsignedInteger cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>::parse_hex_string(
	fixed_uint_str<Allocator> parseMe)
{
	sv converted = static_cast<sv>(parseMe);
	return parse_hex_string(converted);
}

template <typename Char, typename CharTraits, typename Allocator>
constexpr typename cjm::numerics::uint_to_string_helper<Char, CharTraits, Allocator>::dec_lookup_t cjm::numerics::
uint_to_string_helper<Char, CharTraits, Allocator>::get_decimal_lookup()
{
	if constexpr (std::is_same_v<Char, char>)
	{
		dec_lookup_t decimal_char_lookup = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		return decimal_char_lookup;
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		dec_lookup_t decimal_char_lookup = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
		return decimal_char_lookup;
	}
	else if constexpr (std::is_same_v<Char, char16_t>)
	{
		dec_lookup_t decimal_char_lookup = { u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9' };
		return decimal_char_lookup;
	}
	else
	{
		static_assert(std::is_same_v<Char, char32_t>, "Character type supplied not supported.");
		dec_lookup_t decimal_char_lookup = { U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9' };
		return decimal_char_lookup;
	}

}

template <typename Char, typename CharTraits, typename Allocator>
constexpr typename cjm::numerics::uint_to_string_helper<Char, CharTraits, Allocator>::hex_lookup_t cjm::numerics::
uint_to_string_helper<Char, CharTraits, Allocator>::get_hexadecimal_lookup()
{
	if constexpr (std::is_same_v<Char, char>)
	{
		hex_lookup_t hex_char_lookup = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		return hex_char_lookup;
	}
	else if constexpr (std::is_same_v<Char, wchar_t>)
	{
		hex_lookup_t hex_char_lookup = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f' };
		return hex_char_lookup;
	}
	else if constexpr (std::is_same_v<Char, char16_t>)
	{
		hex_lookup_t hex_char_lookup = { u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u'a', u'b', u'c', u'd', u'e', u'f' };
		return hex_char_lookup;
	}
	else
	{
		static_assert(std::is_same_v<Char, char32_t>, "Character type supplied not supported.");
		hex_lookup_t hex_char_lookup = { U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'a', U'b', U'c', U'd', U'e', U'f' };
		return hex_char_lookup;
	}
}


template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::math_functions::int_sign(
	// ReSharper disable once CppParameterNeverUsed
	const fixed_uint<LimbType>& val) noexcept
{
	return fixed_uint<LimbType>(1);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::math_functions::int_gcd(const fixed_uint<LimbType>& first,
	const fixed_uint<LimbType>& second) noexcept
{
	while (second != 0)
	{
		fixed_uint<LimbType> r = first % second;
		first = second;
		second = r;
	}
	return first;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::math_functions::int_lcm(const fixed_uint<LimbType>& first,
	const fixed_uint<LimbType>& second) 
{
	if (first == 0 && second == 0)
	{
		return 0;
	}
	if (first == 0)
	{
		std::swap(first, second);
	}
	fixed_uint<LimbType> product = first * second;
	if (product == 0)
		return 0;
	if (product < first || product < second)
		throw std::overflow_error("Integer overflow occurred.");
	return product / (int_gcd(first, second));
}

template<typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::make_fixed_uint(const LimbType & hi, const LimbType & low) noexcept
{
	return cjm::numerics::fixed_uint<LimbType>{hi, low};
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::make_from_bytes_little_endian(
	byte_array bytes) noexcept
{
	using ret_t = fixed_uint<LimbType>;
	static constexpr size_t byteCount = sizeof(ret_t);
	static_assert(byteCount == bytes.size());
	static constexpr size_t byte_shift_size = CHAR_BIT;
	using byte = unsigned char;
	ret_t ret = 0;
	for (size_t index = 0; index < byteCount; ++index)
	{
		byte b = bytes[index];
		auto temp = static_cast<ret_t>(b);
		temp <<= (index * byte_shift_size);
		ret |= temp;
	}
	return ret;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::make_from_bytes_big_endian(
	byte_array bytes) noexcept
{
	using ret_t = fixed_uint<LimbType>;
	static constexpr size_t byteCount = sizeof(ret_t);
	static_assert(byteCount == bytes.size());
	static constexpr size_t byte_shift_size = CHAR_BIT;
	using byte = unsigned char;
	ret_t ret = 0;
	size_t bitPos = 0;
	for (size_t index = byteCount - 1; index != std::numeric_limits<size_t>::max(); --index)
	{
		byte b = bytes[index];
		auto temp = static_cast<ret_t>(b);
		temp <<= (bitPos++ * byte_shift_size);
		ret |= temp;
	}
	return ret;
}

template<typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::
fixed_uint(const int_part& other) noexcept : m_low{other}, m_high{}
{
	
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(
	const int_part& other) noexcept
{
	m_high = 0;
	m_low = other;
	return *this;	
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(char v) noexcept : 
	m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(short v) noexcept :
	m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(unsigned char v) noexcept 
	: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(unsigned short v) noexcept 
	: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(int v) noexcept
	: m_low{ static_cast<int_part>(v) },
	m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(unsigned v)noexcept : m_low{ v }, m_high{ 0 } {}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(long v) noexcept
	: m_low{ static_cast<int_part>(v) },
	m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(unsigned long v)  noexcept
	: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(long long v) noexcept
	: m_low{ static_cast<int_part>(v) },
	m_high{ v < 0 ? std::numeric_limits<int_part>::max() : 0 } {}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::fixed_uint(unsigned long long v) noexcept
	: m_low{ static_cast<int_part>(v) }, m_high{ 0 } {}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=
(char v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=
(unsigned char v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=
(unsigned short v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(short v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(int v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
	return *this;
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(unsigned v) noexcept 
{
	m_low = static_cast<int_part>(v);
	m_high = 0;
	return *this;
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(long v) noexcept 
{
	m_low = static_cast<int_part>(v);
	m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
	return *this;
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(unsigned long v) noexcept 
{
	m_low = static_cast<int_part>(v);
	m_high = 0;
	return *this;
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(long long v) noexcept 
{
	m_low = static_cast<int_part>(v);
	m_high = v < 0 ? std::numeric_limits<int_part>::max() : 0;
	return *this;
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator=(
	unsigned long long v) noexcept
{
	m_low = static_cast<int_part>(v);
	m_high = 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::operator++(int) noexcept
{
	fixed_uint<LimbType> tmp(*this);
	*this += 1;
	return tmp;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::operator--(int) noexcept
{
	fixed_uint<LimbType> tmp(*this);
	*this -= 1;
	return tmp;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator++() noexcept
{
	*this += 1;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator--() noexcept
{
	*this -= 1;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator+=(
	const fixed_uint& other) noexcept
{
	m_high = other.m_high + m_high;
	m_low = other.m_low + m_low;
	if (m_low < other.m_low)
		++m_high;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator-=(
	const fixed_uint& other) noexcept
{
	//omfg -- this bug was hard to find -- was checking if less than after performing the subtraction
	//i'm pretty sure that would have been ok if i checked for GREATER THAN after the sub
	//but i'll do the bool check first and keep it the same 
	const bool borrow = m_low < other.m_low;
	m_high = m_high - other.m_high;
	m_low = m_low - other.m_low;
	if (borrow)
		--m_high;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator*=(
	const fixed_uint& other) noexcept
{
	int_part aSub = m_low >> int_part_bottom_half_bits;
	int_part a00 = m_low & int_part_bottom_half_bitmask;
	int_part bSub = other.m_low >> int_part_bottom_half_bits;
	int_part b00 = other.m_low & int_part_bottom_half_bitmask;

	m_high = m_high * other.m_low + m_low * other.m_high + aSub * bSub;
	m_low = a00 * b00;
	*this += (static_cast<fixed_uint<LimbType>>(aSub * b00) << int_part_bottom_half_bits);
	*this += (static_cast<fixed_uint<LimbType>>(a00 * bSub) << int_part_bottom_half_bits);
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator
/=(const fixed_uint& other)
{
	if (other == 0)
		throw std::domain_error("Division by zero is illegal.");
	fixed_uint<LimbType> quotient = 0;
	fixed_uint<LimbType> remainder = 0;
	fixed_uint<LimbType>::div_mod_impl(*this, other, &quotient, &remainder);
	return (*this = quotient);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator
%=(const fixed_uint& other)
{
	if (other == 0)
		throw std::domain_error("Modulus by zero is illegal.");
	fixed_uint<LimbType> quotient = 0;
	fixed_uint<LimbType> remainder = 0;
	fixed_uint<LimbType>::div_mod_impl(*this, other, &quotient, &remainder);
	return (*this = remainder);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator<<=(int amount) noexcept
{
	auto absAmount = static_cast<std::uintmax_t>(math_functions::int_abs(amount));
	return (*this <<= absAmount);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator>>=(int amount) noexcept
{
	auto absAmount = static_cast<std::uintmax_t>(math_functions::int_abs(amount));
	return (*this >>= absAmount);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator<<=(
	std::uintmax_t amount) noexcept
{
	//It is undefined behavior to attempt a shift greater than number of bits in an integral type
	assert(amount < std::numeric_limits<fixed_uint<LimbType>>::digits);
	if (amount < int_part_bits)
	{
		if (amount != 0)
		{
			m_high = (m_high << amount) | (m_low >> (int_part_bits - amount));
			m_low = m_low << amount;
		}
		return *this;
	}
	m_high = (m_low << (amount - int_part_bits));
	m_low = 0;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator>>=(
	std::uintmax_t amount) noexcept
{
	//It is undefined behavior to attempt a shift greater than OR EQUAL TO number of bits in an integral type
	assert(amount < std::numeric_limits<fixed_uint<LimbType>>::digits);
	//Todo fixit change order to make copy unnecessary
	auto absAmount = amount;
	auto origHi = m_high;
	if (absAmount < int_part_bits)
	{
		if (absAmount != 0)
		{
			m_high = (m_high >> absAmount);
			m_low = (m_low >> absAmount) | (origHi << (int_part_bits - absAmount));
		}
		return *this;
	}
	m_high = 0;
	m_low = (origHi >> (absAmount - int_part_bits));
	return *this;
}

template <typename LimbType>
template <typename TUint>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator
<<=(TUint amount) noexcept
{
	static_assert(std::is_constructible_v<TUint, uintmax_t> && std::numeric_limits<TUint>::is_integer && !std::numeric_limits<TUint>::is_signed, 
		"TUint must be an unsigned integer for which there exists a conversion to uintmax_t.");
	return (*this <<= static_cast<std::uintmax_t>(amount));
}

template <typename LimbType>
template <typename TUint>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator
>>=(TUint amount) noexcept
{
	static_assert(std::is_constructible_v<TUint, uintmax_t> && std::numeric_limits<TUint>::is_integer && !std::numeric_limits<TUint>::is_signed,
		"TUint must be an unsigned integer for which there exists a conversion to uintmax_t.");
	return (*this >>= static_cast<std::uintmax_t>(amount));
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator&=(
	const fixed_uint& other) noexcept 
{
	m_low &= other.m_low;
	m_high &= other.m_high;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator|=(
	const fixed_uint& other) noexcept
{
	m_low |= other.m_low;
	m_high |= other.m_high;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>& cjm::numerics::fixed_uint<LimbType>::operator^=(
	const fixed_uint& other) noexcept
{
	m_low ^= other.m_low;
	m_high ^= other.m_high;
	return *this;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator bool() const noexcept 
{
	return m_high > 0 || m_low > 0;
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator char() const noexcept 
{
	return static_cast<char>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator signed char() const noexcept
{
	return static_cast<signed char>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator char16_t() const noexcept
{
	return static_cast<char16_t>(m_low);
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator char32_t() const noexcept
{
	return static_cast<char32_t>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator wchar_t() const noexcept 
{
	return static_cast<wchar_t>(m_low);
}
template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator short() const noexcept
{
	return static_cast<short>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator int() const noexcept
{
	return static_cast<int>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator unsigned char() const noexcept 
{
	return static_cast<unsigned char>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator unsigned short() const noexcept 
{
	return static_cast<unsigned short>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator unsigned() const noexcept 
{
	return static_cast<unsigned>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator unsigned long() const noexcept 
{
	return static_cast<unsigned long>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator unsigned long long() const noexcept 
{
	return static_cast<unsigned long long>(m_low);
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType>::operator int_part() const noexcept
{
	return static_cast<int_part>(m_low);
}

template <typename LimbType>
constexpr typename cjm::numerics::fixed_uint<LimbType>::byte_array cjm::numerics::fixed_uint<LimbType>::
to_little_endian_arr() const noexcept
{
	using byte = unsigned char;
	byte_array ret;
	static constexpr size_t shiftAmount = CHAR_BIT;
	for (size_t bytesToShift = 0; bytesToShift < ret.size(); ++bytesToShift)
	{
		auto temp = (*this >> (bytesToShift * shiftAmount));
		auto b = static_cast<byte>(temp);
		ret[bytesToShift] = b;
	}
	return ret;
}

template <typename LimbType>
constexpr typename cjm::numerics::fixed_uint<LimbType>::byte_array cjm::numerics::fixed_uint<LimbType>::
to_big_endian_arr() const noexcept
{
	using byte = unsigned char;
	byte_array ret;
	static constexpr size_t shiftAmount = CHAR_BIT;
	fixed_uint<LimbType> temp = *this;
	for (size_t index = ret.size() - 1; index != std::numeric_limits<size_t>::max(); --index)
	{
		ret[index] = static_cast<byte>(temp);
		temp >>= shiftAmount;
	}
	return ret;
}

template <typename LimbType>
constexpr int cjm::numerics::fixed_uint<LimbType>::fls(const fixed_uint& n)
{
	int_part hi = n.high_part();
	if (hi != 0)
	{
		auto flsHigh = int_part::fls(hi);
		auto sum = flsHigh + int_part_bits;
		return static_cast<int>(sum);
	}
	return static_cast<int>(int_part::fls(n.low_part()));
}

template <typename LimbType>
constexpr void cjm::numerics::fixed_uint<LimbType>::div_mod_impl(fixed_uint dividend, const fixed_uint& divisor,
	fixed_uint* quotient_ret, fixed_uint* remainder_ret)
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

	fixed_uint<LimbType> denominator = divisor;
	fixed_uint<LimbType> quotient = 0;

	const int shift = fls(dividend) - fls(denominator);
	denominator <<= shift;

	for (int i = 0; i <= shift; i++)
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


template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::min() noexcept
{
	return std::numeric_limits<LimbType>::min();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
lowest() noexcept
{
	return std::numeric_limits<LimbType>::lowest();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
max() noexcept
{
	return cjm::numerics::fixed_uint<LimbType>::make_fixed_uint(std::numeric_limits<LimbType>::max(), std::numeric_limits<LimbType>::max());
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
epsilon() noexcept
{
	return std::numeric_limits<LimbType>::epsilon();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
round_error() noexcept
{
	return std::numeric_limits<LimbType>::round_error();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
infinity() noexcept
{
	return std::numeric_limits<LimbType>::infinity();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
quiet_NaN() noexcept
{
	return std::numeric_limits<LimbType>::quiet_NaN();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
signaling_NaN() noexcept
{
	return std::numeric_limits<LimbType>::signaling_NaN();
}

template <typename LimbType>
constexpr cjm::numerics::fixed_uint<LimbType> std::numeric_limits<cjm::numerics::fixed_uint<LimbType>>::
denorm_min() noexcept
{
	return std::numeric_limits<LimbType>::denorm_min();
}

template<char ...Chars>
constexpr cjm::numerics::uint256_t  cjm::numerics::fixed_uint_literals::operator""_u256()
{
	auto charArray = fixed_uint_lit_helper::get_array<Chars...>();
	return fixed_uint_lit_helper::parse_from_char_array<uint256_t, charArray.size()>(charArray);
}

template <typename Target, size_t Size>
constexpr Target cjm::numerics::fixed_uint_literals::fixed_uint_lit_helper::get_decimal_literal(
	std::array<char, Size> arr)
{
	using ret_t = Target;
	static_assert(is_fixed_uint_v<ret_t>, "Illegal return type.");

	ret_t ret = 0;
	auto result = u128_helper::scan_chars_dec(arr);
	if (result.first)
	{
		ret_t exponent = 1;
		size_t length = result.second;
		for (auto i = length - 1; i != std::numeric_limits<size_t>::max();)
		{
			auto temp = u128_helper::get_dec_val(arr, i);
			ret_t retCopy = ret;
			ret += (temp.first * exponent);
			if (ret < retCopy)
				throw std::domain_error("literal too large to fit.");
			exponent *= 10;
			i = temp.second;
		}
		return ret;
	}
	throw std::domain_error("bad decimal literal");
}

template <typename Target, size_t Size>
constexpr Target cjm::numerics::fixed_uint_literals::fixed_uint_lit_helper::get_hex_literal(std::array<char, Size> arr)
{
	using ret_t = Target;
	static_assert(is_fixed_uint_v<ret_t>, "Illegal return type.");

	size_t length = arr.size();
	if (length < 3)
		throw std::domain_error("Bad literal");
	if (arr[0] != '0' || (arr[1] != 'X' && arr[1] != 'x'))
		throw std::domain_error("Only hexadecimal literals are allowed.");

	ret_t value = 0;
	size_t byteCount = 0;
	for (size_t i = length - 1; i > 1;)
	{
		if (byteCount >= sizeof(ret_t))
			throw std::domain_error("The literal is too long.");
		auto pair = u128_helper::get_byte(arr, i);
		ret_t insertMe = static_cast<ret_t>(pair.first) << (static_cast<int>(byteCount++) * CHAR_BIT);
		value |= insertMe;
		i = pair.second;
	}
	return value;	
}
 


template <typename Target, size_t Size>
constexpr Target cjm::numerics::
fixed_uint_literals::fixed_uint_lit_helper::parse_from_char_array(std::array<char, Size> arr)
{
	using ret_t = Target;
	static_assert(is_fixed_uint_v<ret_t>, "Illegal return type.");
	uint128_literals::lit_type lt = u128_helper::get_lit_type(arr);
	ret_t ret{};
	switch (lt)
	{
	default:
	case uint128_literals::lit_type::Illegal:
		throw std::domain_error("Illegal literal: only hexadecimal and decimal are supported.");
	case uint128_literals::lit_type::Decimal:
		ret = get_decimal_literal<ret_t>(arr);
		break;
	case uint128_literals::lit_type::Hexadecimal:
		ret = get_hex_literal<ret_t>(arr);
		break;
	case uint128_literals::lit_type::Zero:
		ret = 0;
		break;
	}
	return ret;
}
template <typename TUInt, typename Char, typename CharTraits, typename Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::numerics::uint_to_string(TUInt convertMe,
	std::ios_base::fmtflags flags)
{
	using string = std::basic_string<Char, CharTraits, Allocator>;
	using deque = std::deque<Char>;
	using helper_t = uint_to_string_helper<Char, CharTraits, Allocator>;
	static_assert(std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t> || std::is_same_v<Char, char16_t>
		|| std::is_same_v<Char, char32_t>, "Specified character type is not supported.");
	static_assert(std::numeric_limits<TUInt>::is_integer && !std::numeric_limits<TUInt>::is_signed,
		"Type must be unsigned integer.");

	string ret;
	helper_t helper = helper_t((flags));
	if (convertMe == 0)
	{
		ret.push_back(helper_t::decimal_lookup[0]);
	}
	else
	{
		deque d = deque((std::numeric_limits<TUInt>::digits10 + 1));
		size_t count = 0;
		while (convertMe != 0)
		{
			Char insertMe = helper(convertMe);
			d.push_front(insertMe);
			++count;
			convertMe /= helper.divisor();
		}
		for (size_t i = 0; i < count; i++)
		{
			ret.push_back(d[i]);
		}
	}
	return ret;
}

template <typename UnsignedInteger, typename Char, typename CharTraits>
template <typename Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::numerics::fixed_uint_parse_helper<UnsignedInteger, Char, CharTraits>
::trim_and_strip(fixed_uint_str<Allocator> trimAndStripMe)
{
	std::basic_string<Char, CharTraits, Allocator> ret;
	auto trimmed = string::trim(std::move(trimAndStripMe));
	auto nonDecimalSeparator = non_decimal_separator();
	for (auto character : trimmed)
	{	
		if (std::find(nonDecimalSeparator.cbegin(), nonDecimalSeparator.cend(), character) == nonDecimalSeparator.cend())
			ret.push_back(character);
	}
	return ret;
}

template <typename Char, typename CharTraits, typename Allocator>
template <typename TUInt>
Char cjm::numerics::uint_to_string_helper<Char, CharTraits, Allocator>::operator()(const TUInt& uint) const
{
	static_assert(std::numeric_limits<TUInt>::is_integer && !std::numeric_limits<TUInt>::is_signed,
		"TUInt type must be an unsigned integer.");
	assert(m_divisor == 16 || m_divisor == 10);
	auto temp = uint % m_divisor;
	auto indexFromVal = static_cast<size_t>(uint % m_divisor);
	Char ret;
	switch (m_divisor)
	{
	case 16:
		ret = hex_lookup[indexFromVal];
		break;
	case 10:
		ret = decimal_lookup[indexFromVal];
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
}

template <typename Char, typename CharTraits, typename Allocator>
cjm::numerics::uint_to_string_helper<Char, CharTraits, Allocator>::
uint_to_string_helper(std::ios_base::fmtflags flags)
{
	switch (flags & std::basic_ios<Char, CharTraits>::basefield)
	{
	case std::basic_ios<Char, CharTraits>::hex:
		m_divisor = 16;
		break;
	case std::basic_ios<Char, CharTraits>::dec:
		m_divisor = 10;
		break;
	default:
		throw std::invalid_argument("Only hexadecimal and decimal are supported.");
	}
}

template <typename LimbType>
template <typename Chars, typename CharTraits, typename Allocator>
cjm::numerics::fixed_uint<LimbType> cjm::numerics::fixed_uint<LimbType>::make_from_string(
	std::basic_string<Chars, CharTraits, Allocator> parseMe)
{
	using ph = fixed_uint_parse_helper<fixed_uint<LimbType>, Chars, CharTraits>;
	auto trimmed = ph::trim_and_strip(parseMe);
	const auto trimmedView = static_cast<std::basic_string_view<Chars, CharTraits>>(trimmed);
	auto parseFormat = ph::get_format(trimmedView);
	fixed_uint<LimbType> ret;
	switch (parseFormat)
	{
	default:
	case fixed_uint_format::Illegal:
		throw std::invalid_argument("Unable to parse an unsigned integer from the supplied string.");
	case fixed_uint_format::Zero:
		ret = 0;
		break;
	case fixed_uint_format::Decimal:
		ret = ph::parse_decimal_string(trimmedView);
		break;
	case fixed_uint_format::Hexadecimal:
		ret = ph::parse_hex_string(trimmedView);
		break;
	}
	return ret;
}

template <typename LimbType>
void cjm::numerics::fixed_uint<LimbType>::instrumented_div_mod(std::basic_ostream<char>& stream, fixed_uint dividend,
	const fixed_uint& divisor, fixed_uint* quotient_ret, fixed_uint* remainder_ret)
{
	using streams::newl;
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
	stream << "BEGIN INSTRUMENTED DIV MOD 256+" << newl;
	stream << "dividend: 0x" << dividend.tostring() << newl;
	stream << "divisor: 0x" << divisor.tostring() << newl;

	auto denominator = divisor;
	auto quotient = static_cast<fixed_uint<LimbType>>(0);

	stream << "denominator: 0x" << denominator.tostring() << newl;
	stream << "quotient: 0x" << quotient.tostring() << newl;

	const auto flsDividend = fls(dividend);
	const auto flsDenominator = fls(denominator);
	const auto shiftAmount = flsDividend - flsDenominator;
	denominator <<= shiftAmount;
	stream << std::dec;
	stream << "DECIMAL FLS RESULTS:" << newl;
	stream << "flsDividend: " << flsDividend << newl;
	stream << "flsDenominator: " << flsDenominator << newl;
	stream << "shiftAmount: " << std::hex << shiftAmount << newl;

	stream << std::hex;
	stream << "HEX FLS RESULTS:" << newl;
	stream << "flsDividend: 0x" << flsDividend << newl;
	stream << "flsDenominator: 0x" << flsDenominator << newl;
	stream << "shiftAmount: 0x" << std::hex << shiftAmount << newl;
	
	
	stream << "denominator <<= 0x" << shiftAmount << " == 0x" << denominator.tostring() << newl;
	stream << std::boolalpha;
	stream << newl << newl << "STARTING CALCULATION LOOP" << newl;
	for (int i = 0; i <= shiftAmount; i++)
	{
		stream << "i = " << std::dec << i << " i <= " << shiftAmount
			<< "= " << (i <= shiftAmount) << std::hex << newl;
		stream << "quotient: 0x" << quotient.tostring() << newl;
		quotient <<= 1;
		stream << "(quotient <<= 1): 0x" << quotient.tostring() << newl;
		stream << "denominator: 0x" << denominator.tostring() << newl;
		bool dividendGreaterThanOrEqualToDenom = dividend >= denominator;
		stream << "dividendGreaterThanOrEqualToDenom: " << dividendGreaterThanOrEqualToDenom << newl;
		if (dividendGreaterThanOrEqualToDenom)
		{
			dividend -= denominator;
			quotient |= 1;
			stream << "dividend -= denominator: 0x" << dividend.tostring() << newl;
			stream << "quotient |= 1: 0x" << quotient.tostring() << newl;
		}
		denominator >>= 1;
		stream << "denominator >>= 1: 0x" << denominator.tostring() << newl;
	}

	stream << "final quotient: 0x" << quotient.tostring() << newl;
	stream << "final remainder (dividend): 0x" << dividend.tostring() << newl;
	*quotient_ret = quotient;
	*remainder_ret = dividend;
	stream << "END INSTRUMENTED DIV MOD 128" << newl;

}

template <typename LimbType>
cjm::numerics::fixed_uint<LimbType>::operator float() const 
{
	static constexpr int exponent = static_cast<int>(static_cast<size_t>(int_part_bits));
	auto low = static_cast<float>(m_low);
	auto high = std::ldexp(static_cast<float>(m_high), exponent);
	return low + high;
}

template <typename LimbType>
cjm::numerics::fixed_uint<LimbType>::operator double() const 
{
	static constexpr int exponent = static_cast<int>(static_cast<size_t>(int_part_bits));
	auto low = static_cast<double>(m_low);
	auto high = std::ldexp(static_cast<double>(m_high), exponent);
	return low + high;
}

template <typename LimbType>
cjm::numerics::fixed_uint<LimbType>::operator long double() const 
{
	static constexpr int exponent = static_cast<int>(static_cast<size_t>(int_part_bits));
	auto low = static_cast<long double>(m_low);
	auto high = std::ldexp(static_cast<long double>(m_high), exponent);
	return low + high;
}


template <typename LimbType>
std::string cjm::numerics::fixed_uint<LimbType>::tostring() const
{
	
		using stream = std::stringstream;
		stream temp;
		auto arr = to_little_endian_arr();
		temp << std::hex;
		for (auto it = arr.crbegin(); it != arr.crend(); ++it)
		{
			temp << (+(*it));
		}
		return temp.str();
	
}

template <typename LimbType>
template <typename Char, typename CharTraits, typename Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::numerics::fixed_uint<LimbType>::to_string(const fixed_uint<LimbType>& item,
	std::ios_base::fmtflags flags)
{
	return uint_to_string<fixed_uint<LimbType>, Char>(item, flags);
}

template <char... Chars>
constexpr std::array<char, sizeof...(Chars)> cjm::numerics::fixed_uint_literals::fixed_uint_lit_helper::get_array()
{
	return u128_helper::get_array<Chars...>();
}




#endif // CJM_FIXED_UINT_INL
