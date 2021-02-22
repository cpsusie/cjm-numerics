#ifndef CJM_NUMERICS_HPP_
#define CJM_NUMERICS_HPP_
#include <cjm/numerics/cjm_configuration.hpp>
#include <cjm/numerics/numerics_configuration.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>

namespace cjm
{

	namespace numerics
	{
		enum class compiler_used : unsigned;
		
		/// <summary>
		/// Serves as a proxy for C++20's std::bit_cast which is not
		/// yet available on Clang.  If std::bit_cast is available,
		/// it simply delegates to std::bit_cast and IS constexpr.
		///
		/// If std::bit_cast is NOT available, uses std::memcpy and
		/// IS NOT constexpr.
		/// </summary>
		/// <typeparam name="To">The type to bit_cast to.</typeparam>
		/// <typeparam name="From">The type to bit_cast from.</typeparam>
		/// <param name="f">the object to bit_cast</param>
		/// <returns>An object of type To that contains the same bit-pattern
		/// as f.</returns>
		/// <remarks>
		/// The concept bit_castable requires:
		///		To and From must have same size and alignment.
		///		To and From must be nothrow default constructible.
		///		To and From must be trivially copyable.
		/// </remarks>
		template<typename To, typename From>
			requires cjm::numerics::concepts::bit_castable<To, From>
		CJM_BIT_CAST_CONST To bit_cast(const From& f) noexcept;

		namespace literals
		{
			
			constexpr size_t operator"" _szt(unsigned long long u) noexcept 
			{
				return static_cast<size_t>(u);
			}		
		}
		
		namespace math_functions
		{
			//todo fixit -- work on making these make more sense in context of library only uint128
			template<typename TInt>
			constexpr std::make_unsigned_t<TInt> int_abs(TInt val) noexcept;

			template<cjm::numerics::concepts::integer TInt>
			constexpr TInt int_sign(TInt val) noexcept;

			template<concepts::unsigned_integer TUInt>
			constexpr TUInt int_gcd(TUInt first, TUInt second) noexcept;

			template<concepts::unsigned_integer TUInt>
			constexpr TUInt int_lcm(TUInt first, TUInt second) noexcept;
			
			template<typename TInt>
			constexpr TInt floor_log2(TInt val);

			template<cjm::numerics::concepts::integer TInt>
			constexpr TInt int_pow(TInt radix, unsigned exponent);

			template<typename TUInt>
			constexpr bool is_uint_power_of_two(TUInt testMe) noexcept;

			constexpr long double ten_to_the_power(int exponent);
			
			
		}

		template<typename Char, typename CharTraits = std::char_traits<Char>>
			requires (cjm::numerics::concepts::char_or_wchar_t_with_traits<Char, CharTraits>)
		std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, compiler_used comp);
		
	}

	
}

namespace std
{
	template<>
	struct hash<cjm::numerics::uint128>;

	/************************************************************************/
	/* Defines numeric limits and various traits for this object
	* to facilitate interoperability with code  that relies on these traits.  */
	/************************************************************************/
	template<>
	class numeric_limits<cjm::numerics::uint128> final
	{
		static constexpr int times_log10_of_two(int x)
		{
			return x * 301'299 / 1'000'000;
		}
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer{ true };
		static constexpr bool is_exact = true;
		static constexpr bool is_bounded = true;
		static constexpr bool has_denorm = std::denorm_absent;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = std::numeric_limits<uint64_t>::has_quiet_NaN;
		static constexpr bool has_signaling_NaN = std::numeric_limits<uint64_t>::has_signaling_NaN;
		static constexpr bool has_denorm_loss = std::numeric_limits<uint64_t>::has_denorm_loss;
		static constexpr std::float_round_style round_style = std::numeric_limits<uint64_t>::round_style;
		static constexpr bool is_arithmetic = true;
		static constexpr bool is_iec559 = std::numeric_limits<uint64_t>::is_iec559;
		static constexpr bool is_modulo = std::numeric_limits<uint64_t>::is_modulo;
		static constexpr int digits = std::numeric_limits<uint64_t>::digits * 2;
		static constexpr int digits10 = digits * 301'299 / 1'000'000;
		static constexpr int max_digits10 = std::numeric_limits<uint64_t>::max_digits10;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int max_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = true;

		static constexpr cjm::numerics::uint128 min() noexcept;

		static constexpr cjm::numerics::uint128 lowest() noexcept;

		static constexpr cjm::numerics::uint128 max() noexcept;

		static constexpr cjm::numerics::uint128 epsilon() noexcept;

		static constexpr cjm::numerics::uint128 round_error() noexcept;

		static constexpr cjm::numerics::uint128 infinity() noexcept;

		static constexpr cjm::numerics::uint128 quiet_NaN() noexcept;

		static constexpr cjm::numerics::uint128 signaling_NaN() noexcept;

		static constexpr cjm::numerics::uint128 denorm_min() noexcept;
	};
}
#include <cjm/numerics/numerics.inl>
#endif
