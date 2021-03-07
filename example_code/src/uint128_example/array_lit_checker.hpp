#ifndef CJM_EXPERIMENTAL_ARRAY_LIT_CHECKER_HPP_
#define CJM_EXPERIMENTAL_ARRAY_LIT_CHECKER_HPP_
#include <cjm/numerics/uint128.hpp>
#include <array>
#include <optional>


namespace cjm::experimental::array_lit_checker
{
    using uint128_t = cjm::numerics::uint128;

	using lit_type = cjm::numerics::uint128_literals::lit_type;

	template<concepts::unsigned_integer Ui>
    constexpr std::array<char, std::numeric_limits<Ui>::digits10 + 1> get_max_decimal();
	
    template<concepts::unsigned_integer Ui>
    constexpr std::array<char, std::numeric_limits<Ui>::digits10 + 1> max_decimal_digits_v = get_max_decimal<Ui>();

    template<concepts::unsigned_integer Ui>
    constexpr size_t max_hex_digits_v = std::numeric_limits<Ui>::digits / 4;
	
    template<char... Chars>
    constexpr std::optional<size_t> count_decimal_chars();

	template<char... Chars>
    constexpr std::optional<size_t> count_hex_chars();

    template<concepts::unsigned_integer Ui, char... Chars>
    constexpr bool validate_decimal_size();
	
    template<concepts::unsigned_integer Ui>
	constexpr std::array<char, std::numeric_limits<Ui>::digits10 + 1> get_max_decimal()
    {
        constexpr Ui max = std::numeric_limits<Ui>::max();
        std::array<char, std::numeric_limits<Ui>::digits10 + 1> ret{};
    	Ui current = max;
        bool done = false;
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
    constexpr bool are_all_chars_0();
	
	template<char... Chars>
	constexpr lit_type get_lit_type()
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
            return uint128_lit_helper::is_legal_hex_char(arr[2]) ? lit_type::Hexadecimal : lit_type::Illegal;
        if (arr[0] == '0')
            return lit_type::Illegal;
        return arr[0] >= 0x30 && arr[0] <= 0x39 ? lit_type::Decimal : lit_type::Illegal;
    }

	template<char... Chars>
    constexpr std::optional<size_t> count_decimal_chars()
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
	constexpr lit_type get_chars()
	{
        constexpr lit_type the_lit_type = get_lit_type<Chars...>();
		return the_lit_type;
	}

    template<char... Chars>
    constexpr bool are_all_chars_0()
    {
        std::array<char, sizeof...(Chars)> arr{ Chars... };
        for (char c : arr)
        {
            if (c != 0)
                return false;
        }
        return true;		
    }

    template<concepts::unsigned_integer Ui, char... Chars>
    constexpr bool validate_decimal_size()
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
    constexpr std::optional<size_t> count_hex_chars()
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
			for (size_t j = 0; j < ok_chars.size(); ++j )
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

	
}

#endif
