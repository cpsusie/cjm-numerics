#ifndef CJM_NUMERICS_HPP_
#define CJM_NUMERICS_HPP_
#include "numerics_configuration.hpp"



namespace cjm
{
	
	namespace numerics
	{
		namespace internal
		{
			//alternate declarations for cjm_intrinsic_macros ... never defined because never used but need something that won't blow compiler up
			//when examining untaken if constexpr branch.
			extern unsigned char cjm_badrev_bitscan_64(unsigned long* index, std::uint64_t mask);
			extern std::uint64_t cjm_bad_lzcnt_64(std::uint64_t mask);
			extern std::uint64_t cjm_bad_umul128(std::uint64_t multiplicand, std::uint64_t multiplicand_two, std::uint64_t* carry);
			extern std::uint64_t cjm_bad_udiv128(std::uint64_t high_dividend, std::uint64_t low_dividend, std::uint64_t divisor, std::uint64_t* remainder);
			extern std::uint64_t cjm_bad_shiftleft128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
			extern std::uint64_t cjm_bad_shiftright128(std::uint64_t low, std::uint64_t high, unsigned char shift_amount);
		}
		class uint128;
        constexpr bool has_cpp20_bitops =
#ifdef CJM_HAS_BITOPS
                true;
#else
        false
#endif
		constexpr bool constexpr_bit_casting =
#ifdef __cpp_lib_bit_cast
			true;
#else
			false;
#endif
	    constexpr bool has_msc_x64 =
#ifdef CJM_MSC_X64
        true;
#else
	    false;
#endif
	    constexpr bool has_msc =
#ifdef CJM_MSC
    true;
#else
	    false;
#endif
	    constexpr bool has_intrinsic_u128 =
#ifdef __SIZEOF_INT128__
        true;
		using uint128_align_t = unsigned __int128;
		using natuint128_t = unsigned __int128;
#ifndef CJM_HAVE_BUILTIN_128
#define CJM_HAVE_BUILTIN_128
#endif
#else
	    false;
		namespace internal
		{
			struct alignas(alignof(std::uint64_t) * 2) cjm_align {
				std::uint64_t m_low;
				std::uint64_t m_high;
			};
		}
		using uint128_align_t = std::conditional_t<has_msc_x64, internal::cjm_align, std::uint64_t>;
		using natuint128_t = uint128;
#ifdef CJM_HAVE_BUILTIN_128
#undef CJM_HAVE_BUILTIN_128
#endif
#endif
	    constexpr size_t uint128_align = alignof(uint128_align_t);

		
		

		enum class uint128_calc_mode : std::uint8_t
        {
		    default_eval = 0x00,
		    msvc_x64,
		    intrinsic_u128,
        };
		constexpr uint128_calc_mode init_eval_mode() noexcept;

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
			template<typename TInt>
			constexpr std::make_unsigned_t<TInt> int_abs(TInt val) noexcept;

			template<typename TInt>
			constexpr TInt int_sign(TInt val) noexcept;

			template<typename TInt>
			constexpr TInt int_gcd(TInt first, TInt second) noexcept;

			template<typename TInt>
			constexpr TInt int_lcm(TInt first, TInt second);
			

			template<typename TInt>
			constexpr TInt floor_log2(TInt val);

			template<typename TInt>
			constexpr TInt int_pow(TInt radix, unsigned exponent);

			template<typename TUInt>
			constexpr bool is_uint_power_of_two(TUInt testMe) noexcept;

			constexpr long double ten_to_the_power(int exponent);
			
			
		}
		#pragma warning(push)
		#pragma warning (disable:4068) 
        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "UnreachableCode"
        constexpr uint128_calc_mode init_eval_mode() noexcept
        {
            if constexpr (has_intrinsic_u128)
            {
                return uint128_calc_mode::intrinsic_u128;
            }
            else if constexpr (has_msc_x64)
            {
                return uint128_calc_mode::msvc_x64;
            }
            else
            {
                return uint128_calc_mode::default_eval;
            }
        }
        #pragma clang diagnostic pop
		#pragma warning(pop)
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
	class numeric_limits<cjm::numerics::uint128>
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
		static constexpr int digits = CHAR_BIT * (sizeof(std::uint64_t) + sizeof(std::uint64_t));
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

#include "numerics.inl"
#endif
