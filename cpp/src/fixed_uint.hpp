#ifndef CJM_FIXED_UINT_HPP
#define CJM_FIXED_UINT_HPP
#include "uint128.hpp"
#include <deque>
#include <algorithm>
namespace cjm::numerics
{



using uint256_t = fixed_uint<uint128>;
using uint512_t = fixed_uint<uint256_t>;
using uint1024_t = fixed_uint<uint512_t>;
using uint2048_t = fixed_uint<uint1024_t>;
using uint4096_t = fixed_uint<uint2048_t>;
using uint8192_t = fixed_uint<uint4096_t>;

template<typename T>
struct is_fixed_uint : std::false_type{};

template<typename LimbType>
struct is_fixed_uint<fixed_uint<LimbType>> : std::true_type {};

template<typename T>
struct is_uint128 : std::false_type {};

template<>
struct is_uint128<uint128> : std::true_type{};

template<typename T>
constexpr bool is_fixed_uint_v{ is_fixed_uint<T>::value };

template<typename T>
constexpr bool is_uint128_v{ is_uint128<T>::value };

template<typename LimbType>
struct is_valid_limb_type : std::conditional_t<(is_fixed_uint_v<LimbType> || 
	is_uint128_v<LimbType>), std::true_type, std::false_type> {};

template<typename LimbType>
constexpr bool is_valid_limb_type_v{ is_valid_limb_type<LimbType>::value };

//Unary operators-- +, -, ~, !
template<typename LimbType>
constexpr fixed_uint<LimbType> operator-(const fixed_uint<LimbType>& operand) noexcept;
template<typename LimbType>
constexpr fixed_uint<LimbType> operator+(const fixed_uint<LimbType>& operand) noexcept;
template<typename LimbType>
constexpr fixed_uint<LimbType> operator~(const fixed_uint<LimbType>& operand) noexcept;
template<typename LimbType>
constexpr fixed_uint<LimbType> operator!(const fixed_uint<LimbType>& operand) noexcept;

//bit shift operators
template<typename LimbType>
constexpr fixed_uint<LimbType> operator>>(fixed_uint<LimbType> lhs, int amount) noexcept;
template<typename LimbType>
constexpr fixed_uint<LimbType> operator<<(fixed_uint<LimbType> lhs, int amount) noexcept;

//bit shift operators
template<typename LimbType, typename TUInt>
constexpr fixed_uint<LimbType> operator>>(fixed_uint<LimbType> lhs, TUInt amount) noexcept;
template<typename LimbType, typename TUInt>
constexpr fixed_uint<LimbType> operator<<(fixed_uint<LimbType> lhs, TUInt amount) noexcept;

template<typename TUInt, typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
	std::basic_string<Char, CharTraits, Allocator> uint_to_string(TUInt convertMe, std::ios_base::fmtflags flags);

enum class fixed_uint_format
{
	Illegal = 0,
	Zero,
	Hexadecimal,
	Decimal
};

template<typename UnsignedInteger, typename Char, typename CharTraits = std::char_traits<Char>> 
struct fixed_uint_parse_helper
{
	static constexpr size_t NumberOfHexTags = 2;
	using sv = std::basic_string_view<Char, CharTraits>;
	template <typename Allocator = std::allocator<Char>>
	using fixed_uint_str = std::basic_string<Char, CharTraits, Allocator>;
	using hextag_array = std::array<sv, NumberOfHexTags>;
	static_assert(std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t> ||
		std::is_same_v<Char, char16_t> || std::is_same_v<Char, char32_t>,
		"Character type not supported.");
	static_assert(std::numeric_limits<UnsignedInteger>::is_integer && 
		!std::numeric_limits<UnsignedInteger>::is_signed && 
		std::numeric_limits<UnsignedInteger>::is_modulo && 
		std::numeric_limits<UnsignedInteger>::is_bounded,
		"UnsignedInteger must be a fixed precision modulo unsigned integer.");
	static constexpr size_t max_hex_chars = (sizeof(UnsignedInteger) * CHAR_BIT) / 4;
	static constexpr Char zero_char();

	static constexpr sv non_decimal_separator();
	static constexpr sv decimal_separator();
	static constexpr std::uint8_t get_hex_value(Char c);
	static constexpr std::uint8_t get_decimal_value(Char c);

	static constexpr fixed_uint_format get_format(const sv& string);
	static constexpr bool is_legal_hex_char(Char c);
	static constexpr bool is_legal_dec_char(Char c);
	template<typename Allocator = std::allocator<Char>>
	static fixed_uint_str<Allocator> trim_and_strip(fixed_uint_str<Allocator> trimAndStripMe);

	static constexpr size_t legal_hex_char_count(const sv& string);

	static constexpr hextag_array get_hex_tags();

	static UnsignedInteger parse_decimal_string(const sv& decimalString);
	static UnsignedInteger parse_hex_string(const sv& hexString);
	template<typename Allocator = std::allocator<Char>>
	static UnsignedInteger parse_decimal_string(fixed_uint_str<Allocator> parseMe);
	template<typename Allocator = std::allocator<Char>>
	static UnsignedInteger parse_hex_string(fixed_uint_str<Allocator> parseMe);
};

template<typename Char, typename CharTraits, typename Allocator>
struct uint_to_string_helper
{
	using string = std::basic_string<Char, CharTraits, Allocator>;
	using dec_lookup_t = std::array<Char, 10>;
	using hex_lookup_t = std::array<Char, 16>;
private:
	static constexpr dec_lookup_t get_decimal_lookup();
	static constexpr hex_lookup_t get_hexadecimal_lookup();
	unsigned char m_divisor;
public:
	template<typename TUInt>
	Char operator()(const TUInt& uint) const;
	unsigned char divisor() const { return m_divisor; }
	uint_to_string_helper() = delete;
	explicit uint_to_string_helper(std::ios_base::fmtflags flags);
	uint_to_string_helper(uint_to_string_helper&& other) noexcept = default;
	uint_to_string_helper(const uint_to_string_helper& other) = default;
	uint_to_string_helper& operator=(const uint_to_string_helper& other) = default;
	uint_to_string_helper& operator=(uint_to_string_helper&& other) noexcept = default;
	~uint_to_string_helper() = default;

	static constexpr dec_lookup_t decimal_lookup = get_decimal_lookup();
	static constexpr hex_lookup_t hex_lookup = get_hexadecimal_lookup();
};
namespace math_functions
{
	template<typename LimbType>
	constexpr fixed_uint<LimbType> int_sign(const fixed_uint<LimbType>& val) noexcept;

	template<typename LimbType>
	constexpr fixed_uint<LimbType> int_gcd(const fixed_uint<LimbType>& first, const fixed_uint<LimbType>& second) noexcept;

	template<typename LimbType>
	constexpr fixed_uint<LimbType> int_lcm(const fixed_uint<LimbType>& first, const fixed_uint<LimbType>& second);

}
template<typename LimbType>
class fixed_uint
{
public:
	friend class fixed_uint<fixed_uint<LimbType>>;
	using int_part = LimbType;
	static constexpr size_t byte_array_size{ LimbType::byte_array_size + LimbType::byte_array_size };

	using byte_array = std::array<unsigned char, byte_array_size>;

	static_assert(is_valid_limb_type_v<LimbType>, "Invalid limb type detected.  Must be uint128 or a smaller fixeduint.");
	template<typename Chars, typename CharTraits = std::char_traits<Chars>, typename Allocator = std::allocator<Chars>>
	static fixed_uint make_from_string(std::basic_string<Chars, CharTraits, Allocator> parseMe);
	static constexpr fixed_uint make_fixed_uint(const LimbType& hi, const LimbType& low) noexcept;
	static constexpr fixed_uint make_from_bytes_little_endian(byte_array bytes) noexcept;
	static constexpr fixed_uint make_from_bytes_big_endian(byte_array bytes) noexcept;
	static constexpr int_part int_part_bits{ sizeof(int_part) * CHAR_BIT };
	static constexpr int_part int_part_bottom_half_bits{ int_part_bits / 2 };
	static_assert(int_part_bottom_half_bits <= std::numeric_limits<int>::max());
	static constexpr int_part int_part_bottom_half_bitmask{ std::numeric_limits<int_part>::max() >> static_cast<int>(int_part_bottom_half_bits) };

	static void instrumented_div_mod(std::basic_ostream<char>& stream, fixed_uint dividend, const fixed_uint& divisor,
		fixed_uint* quotient_ret, fixed_uint* remainder_ret);

	constexpr fixed_uint() noexcept : m_low{}, m_high{} {}
	constexpr fixed_uint(const fixed_uint& other) noexcept = default;
	constexpr fixed_uint(fixed_uint&& other) noexcept = default;
	constexpr fixed_uint& operator=(const fixed_uint& other) noexcept = default;
	constexpr fixed_uint& operator=(fixed_uint&& other) noexcept = default;
	~fixed_uint() noexcept = default;

	constexpr fixed_uint(const int_part& other) noexcept;
	constexpr fixed_uint& operator=(const int_part& other) noexcept;
	constexpr fixed_uint(char v) noexcept;
	constexpr fixed_uint(unsigned char v) noexcept;
	constexpr fixed_uint(short v) noexcept;
	constexpr fixed_uint(unsigned short v) noexcept;
	constexpr fixed_uint(int v) noexcept;
	constexpr fixed_uint(unsigned int v) noexcept;
	constexpr fixed_uint(long v) noexcept;
	constexpr fixed_uint(unsigned long v) noexcept;
	constexpr fixed_uint(long long v) noexcept;
	constexpr fixed_uint(unsigned long long v) noexcept;
	// Assignment operators from arithmetic types
	constexpr fixed_uint& operator=(char v) noexcept;
	constexpr fixed_uint& operator=(unsigned char v) noexcept;
	constexpr fixed_uint& operator=(unsigned short v) noexcept;
	constexpr fixed_uint& operator=(short v) noexcept;
	constexpr fixed_uint& operator=(int v) noexcept;
	constexpr fixed_uint& operator=(unsigned int v) noexcept;
	constexpr fixed_uint& operator=(long v) noexcept;
	constexpr fixed_uint& operator=(unsigned long v) noexcept;
	constexpr fixed_uint& operator=(long long v) noexcept;
	constexpr fixed_uint& operator=(unsigned long long v) noexcept;


	//Increment and decrement operators
	constexpr fixed_uint operator++(int) noexcept;
	constexpr fixed_uint operator--(int) noexcept;
	constexpr fixed_uint& operator++() noexcept;
	constexpr fixed_uint& operator--() noexcept;

	//Compound arithmetic-assignment operators
	constexpr fixed_uint& operator+=(const fixed_uint& other) noexcept;
	constexpr fixed_uint& operator-=(const fixed_uint& other) noexcept;
	constexpr fixed_uint& operator*=(const fixed_uint& other) noexcept;
	constexpr fixed_uint& operator/=(const fixed_uint& other);
	constexpr fixed_uint& operator%=(const fixed_uint& other);
	//compound bit-twiddling-assignment operators
	constexpr fixed_uint& operator<<=(int amount) noexcept;
	constexpr fixed_uint& operator>>=(int amount) noexcept;
	constexpr fixed_uint& operator<<=(std::uintmax_t amount) noexcept;
	constexpr fixed_uint& operator>>=(std::uintmax_t amount) noexcept;
	template<typename TUint>
	constexpr fixed_uint& operator <<=(TUint amount) noexcept;
	template<typename TUint>
	constexpr fixed_uint& operator >>=(TUint amount) noexcept;
	constexpr fixed_uint& operator&=(const fixed_uint& other) noexcept;
	constexpr fixed_uint& operator|=(const fixed_uint& other) noexcept;
	constexpr fixed_uint& operator^=(const fixed_uint& other) noexcept;

	//binary arithmetic and bitwise
	friend constexpr fixed_uint operator+(fixed_uint lhs, const fixed_uint& rhs) noexcept { return (lhs += rhs); }
	friend constexpr fixed_uint operator-(fixed_uint lhs, const fixed_uint& rhs) noexcept { return (lhs -= rhs); }
	friend constexpr fixed_uint operator*(fixed_uint lhs, const fixed_uint& rhs) noexcept { return (lhs *= rhs); }
	friend constexpr fixed_uint operator/(fixed_uint lhs, const fixed_uint& rhs) { return (lhs /= rhs); }
	friend constexpr fixed_uint operator%(fixed_uint lhs, const fixed_uint& rhs) { return (lhs %= rhs); }
	friend constexpr fixed_uint operator&(fixed_uint lhs, const fixed_uint& rhs) { return (lhs &= rhs); }
	friend constexpr fixed_uint operator|(fixed_uint lhs, const fixed_uint& rhs) { return (lhs |= rhs); }
	friend constexpr fixed_uint operator^(fixed_uint lhs, const fixed_uint& rhs) { return (lhs ^= rhs); }

	//comparison operators
	friend constexpr bool operator==(const fixed_uint& lhs, const fixed_uint& rhs) noexcept
	{
		return lhs.low_part() == rhs.low_part() && lhs.high_part() == rhs.high_part();
	}
	friend constexpr bool operator!= (const fixed_uint& lhs, const fixed_uint& rhs) noexcept { return !(lhs == rhs);}
	friend constexpr bool operator> (const fixed_uint& lhs, const fixed_uint& rhs)
	{
		return (lhs.high_part() == rhs.high_part()) ?
			(lhs.low_part() > rhs.low_part()) : (lhs.high_part() > rhs.high_part());
	}
	friend constexpr bool operator< (const fixed_uint& lhs, const fixed_uint& rhs) 
	{
		return (lhs.high_part() == rhs.high_part()) ?
			(lhs.low_part() < rhs.low_part()) : (lhs.high_part() < rhs.high_part());
	}
	friend constexpr bool operator>= (const fixed_uint& lhs, const fixed_uint& rhs) { return !(lhs < rhs); }
	friend constexpr bool operator<= (const fixed_uint& lhs, const fixed_uint& rhs) { return !(lhs > rhs); }	

	//stream insertion op
	template<typename Char, typename CharTraits, typename Allocator = std::allocator<Char>>
	friend std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, const fixed_uint<LimbType>& v)
	{
		
		using ios = std::basic_ios<Char, CharTraits>;
		using iosflags = typename ios::fmtflags;
		iosflags flags = os.flags();
		auto rep = to_string<Char, CharTraits, Allocator>(v, flags);
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
	  

	//conversion operators
	constexpr explicit operator bool() const noexcept;
	constexpr explicit operator char() const noexcept;
	constexpr explicit operator signed char() const noexcept;
	constexpr explicit operator char16_t() const noexcept;
	constexpr explicit operator char32_t() const noexcept;
	constexpr explicit operator wchar_t() const noexcept;
	constexpr explicit operator short() const noexcept;
	constexpr explicit operator int() const noexcept;
	constexpr explicit operator unsigned char() const noexcept;
	constexpr explicit operator unsigned short() const noexcept;
	constexpr explicit operator unsigned int() const noexcept;
	constexpr explicit operator unsigned long() const noexcept;
	constexpr explicit operator unsigned long long() const noexcept;
	constexpr explicit operator int_part() const noexcept;
	explicit operator float() const;
	explicit operator double() const;
	explicit operator long double() const;

	constexpr int_part low_part() const noexcept { return m_low; }
	constexpr int_part high_part() const noexcept { return m_high; }

	constexpr byte_array to_little_endian_arr() const noexcept;
	constexpr byte_array to_big_endian_arr() const noexcept;

	//REMOVE AFTER FIXING DIVISION
	std::string tostring() const;
	

private:	

	constexpr fixed_uint(const LimbType& hi, const LimbType& low) noexcept : m_low{ low }, m_high{ hi } {}

	static constexpr int fls(const fixed_uint& n);
	static constexpr void div_mod_impl(fixed_uint dividend, const fixed_uint& divisor,
		fixed_uint* quotient_ret, fixed_uint* remainder_ret);
	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
	static std::basic_string<Char, CharTraits, Allocator> to_string(const fixed_uint<LimbType>& item, std::ios_base::fmtflags flags);
	
#if CJM_NUMERICS_LITTLE_ENDIAN
	int_part m_low;
	int_part m_high;
#else //BIG ENDIAN
	int_part m_high;
	int_part m_low;
#endif

}; //fixed_uint<LimbType>
} // cjm::numerics

namespace std
{
	template<typename LimbType>
	class numeric_limits<cjm::numerics::fixed_uint<LimbType>>
	{
		static constexpr int times_log10_of_two(int x)
		{
			return static_cast<int>(static_cast<uintmax_t>(x) * 301'299u / 1'000'000u);
		}

	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer{ true };
		static constexpr bool is_exact = true;
		static constexpr bool is_bounded = true;
		static constexpr bool has_denorm = std::denorm_absent;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = std::numeric_limits<LimbType>::has_quiet_NaN;
		static constexpr bool has_signaling_NaN = std::numeric_limits<LimbType>::has_signaling_NaN;
		static constexpr bool has_denorm_loss = std::numeric_limits<LimbType>::has_denorm_loss;
		static constexpr std::float_round_style round_style = std::numeric_limits<LimbType>::round_style;
		static constexpr bool is_arithmetic = true;
		static constexpr bool is_iec559 = std::numeric_limits<LimbType>::is_iec559;
		static constexpr bool is_modulo = std::numeric_limits<LimbType>::is_modulo;
		static constexpr int digits = CHAR_BIT * (sizeof(LimbType) + sizeof(LimbType));
		static constexpr int digits10{ times_log10_of_two(digits) };
		static constexpr int max_digits10 = std::numeric_limits<LimbType>::max_digits10;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int max_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = true;

		static constexpr cjm::numerics::fixed_uint<LimbType> min() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> lowest() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> max() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> epsilon() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> round_error() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> infinity() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> quiet_NaN() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> signaling_NaN() noexcept;

		static constexpr cjm::numerics::fixed_uint<LimbType> denorm_min() noexcept;		
	};
}//std

namespace cjm::numerics::fixed_uint_literals
{
	template<char... Chars>
	constexpr uint256_t operator"" _u256();

	template<char... Chars>
	constexpr uint512_t operator"" _u512();

	template<char... Chars>
	constexpr uint1024_t operator"" _u1024();

	template<char... Chars>
	constexpr uint2048_t operator"" _u2048();

	template<char... Chars>
	constexpr uint4096_t operator"" _u4096();

	template<char... Chars>
	constexpr uint8192_t operator"" _u8192();

	template<size_t ArraySize>
	struct fixed_uint_target_determiner
	{
		static_assert(math_functions::is_uint_power_of_two(ArraySize) && ArraySize >= 256, "Size must be a power of two greater than or equal to 256.");
		using target_type = void;
	};

	template<>
	struct fixed_uint_target_determiner<256>
	{
		using target_type = uint256_t;
	};

	template<>
	struct fixed_uint_target_determiner<512>
	{
		using target_type = uint512_t;
	};

	template<>
	struct fixed_uint_target_determiner<1024>
	{
		using target_type = uint1024_t;
	};

	template<>
	struct fixed_uint_target_determiner<2048>
	{
		using target_type = uint2048_t;
	};

	template<>
	struct fixed_uint_target_determiner<4096>
	{
		using target_type = uint4096_t;
	};

	template<>
	struct fixed_uint_target_determiner<8192>
	{
		using target_type = uint8192_t;
	};

	class fixed_uint_lit_helper
	{
		using u128_helper = uint128_literals::uint128_lit_helper;		

		template<char... Chars>
		friend constexpr uint256_t operator"" _u256();

		template<char... Chars>
		friend constexpr uint512_t operator"" _u512();

		template<char... Chars>
		friend constexpr uint1024_t operator"" _u1024();

		template<char... Chars>
		friend constexpr uint2048_t operator"" _u2048();

		template<char... Chars>
		friend constexpr uint4096_t operator"" _u4096();

		template<char... Chars>
		friend constexpr uint8192_t operator"" _u8192();

		template <typename Target, size_t Size>
		static constexpr Target get_decimal_literal(
			std::array<char, Size> arr);

		template <typename Target, size_t Size>
		static constexpr Target get_hex_literal(std::array<char, Size> arr);


	public:

		template<typename Target, size_t Size>
		static constexpr Target parse_from_char_array(std::array<char, Size> arr);

		template<char... Chars>
		static constexpr std::array<char, sizeof...(Chars)> get_array();

		fixed_uint_lit_helper() = delete;
		~fixed_uint_lit_helper() = delete;
		fixed_uint_lit_helper(const fixed_uint_lit_helper& other) = delete;
		fixed_uint_lit_helper(fixed_uint_lit_helper&& other) noexcept = delete;
		fixed_uint_lit_helper& operator=(const fixed_uint_lit_helper& other) = delete;
		fixed_uint_lit_helper& operator=(fixed_uint_lit_helper&& other) noexcept = delete;
	};

	
}//fixed_uint_literals

#include "fixed_uint.inl"
#endif // CJM_FIXED_UINT_HPP
