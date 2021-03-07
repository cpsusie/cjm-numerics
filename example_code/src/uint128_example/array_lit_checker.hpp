#ifndef CJM_EXPERIMENTAL_ARRAY_LIT_CHECKER_HPP_
#define CJM_EXPERIMENTAL_ARRAY_LIT_CHECKER_HPP_
#include <cjm/numerics/uint128.hpp>
#include <array>
#include <optional>


namespace cjm::experimental::array_lit_checker
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
				static constexpr std::array<char, sizeof...(Chars)> reverse_array();
				static constexpr std::array<char, sizeof...(Chars)> reversed_array_val = reverse_array();
				static constexpr std::array<char, sizeof...(Chars)> array_val{ Chars... };
		};

		template<char... Chars>
			requires (sizeof...(Chars) > 0)
		constexpr std::array<char, sizeof...(Chars)> array_retrieval_helper<Chars...>::reverse_array()
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

    using uint128_t = cjm::numerics::uint128;

	using lit_type = cjm::numerics::uint128_literals::lit_type;

	template<char... Chars>
		requires (sizeof...(Chars) > 0)
	constexpr uint128_t operator"" _tu128() noexcept;

	template<lit_type LiteralType>
	requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
	constexpr std::array<std::optional<unsigned short>, 256u> init_digit_lookup();



	template<lit_type LiteralType>
		requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
	constexpr std::array<std::optional<unsigned short>, 256u>  digit_lookup_v = init_digit_lookup<LiteralType>();

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

    template <concepts::unsigned_integer Ui, char... Chars>
    constexpr std::optional<Ui> parse_literal();

	template<concepts::unsigned_integer Ui, size_t Digits, lit_type LiteralType, char... Chars>
		requires (sizeof...(Chars) > 0 && sizeof...(Chars) >= Digits && Digits > 0 && (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal ))
	constexpr std::optional<Ui> execute_literal_parse();
	
    template<concepts::unsigned_integer Ui>
	constexpr std::array<char, std::numeric_limits<Ui>::digits10 + 1> get_max_decimal()
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

	template<concepts::unsigned_integer Ui, size_t Digits, lit_type LiteralType, char... Chars>
		requires (sizeof...(Chars) > 0 && sizeof...(Chars) >= Digits && Digits > 0 && (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal ))
	constexpr std::optional<Ui> execute_literal_parse()
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
	constexpr std::optional<Ui> parse_literal()
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



	template<lit_type LiteralType>
	requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
	constexpr std::array<std::optional<unsigned short>, 256u> init_digit_lookup()
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

	template<char... Chars>
		requires (sizeof...(Chars) > 0)
	constexpr uint128_t operator"" _tu128() noexcept
	{
		constexpr std::optional<uint128_t> result = parse_literal<uint128_t, Chars...>();
		static_assert(result.has_value(), "This literal is not a valid decimal or hexadecimal uint128_t.");
		return *result;
	}
}

#endif
