#ifndef CJM_INT128_TEST_GEN_HPP_
#define CJM_INT128_TEST_GEN_HPP_
#include <utility>
#include <random>
#include <memory>
#include <vector>
#include <functional>
#include <optional>
#include <cstdint>
#include "int128_tests.hpp"
#include "cjm_numeric_concepts.hpp"
#include "uint128.hpp"
#include <type_traits>
#include <bit>
#include <algorithm>
#include <limits>
#include <concepts>
#include <boost/multiprecision/cpp_int.hpp>

namespace cjm::uint128_tests::generator
{
	static_assert(sizeof(std::uintptr_t) >= 8 || sizeof(std::uintptr_t) == 4,
		"Only 32 and 64 bit systems are supported.");
	namespace internal
	{

		class rgen_impl;
	}
	class rgen;
}

namespace std
{
	template<>
	struct hash<cjm::uint128_tests::generator::rgen>
	{
		[[nodiscard]] size_t operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept;
		
	};
}

namespace cjm::uint128_tests::generator
{
	using uint128_t = numerics::uint128;
	using ctrl_uint128_t = boost::multiprecision::uint128_t;
	namespace concepts
	{
		template<typename UnsignedInteger>
		concept up_to_ui128 = std::numeric_limits<UnsignedInteger>::is_integer &&
			std::numeric_limits<UnsignedInteger>::digits <= 128 && (!std::numeric_limits<UnsignedInteger>::is_signed);

		template<typename Invocable>
		concept binary_op_producer = requires (Invocable func, const rgen & gen, binary_operation<uint128_t, ctrl_uint128_t> op)
		{
			{op = func(gen) };
			
		};
	}

	template<concepts::up_to_ui128 UnsignedInteger>
		uint128_t create_random_in_range(const rgen& rgen);

	template<concepts::up_to_ui128 UnsignedInteger>
	int create_shift_param_for_type(const rgen& rgen);

	
	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t> create_compare_op(const rgen& rgen);
	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t> create_shift_op(const rgen& rgen);
	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t> create_bw_op(const rgen & rgen);
	
	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);
	
	[[nodiscard]] std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
		create_division_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);

	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

	[[nodiscard]] std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
		create_modulus_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);


	
	enum class binary_op_category
	{
		any,
		bitwise,
		bitshift,
		add_subtract_multiply,
		div_mod,
		compare,
	};
	
	[[nodiscard]] uint128_t create_random_in_range(const rgen& rgen, size_t byte_limit);
	class rgen final
	{
	public:
		[[nodiscard]] bool good() const { return static_cast<bool>(m_gen); }
		rgen();
		rgen(const rgen& other) = delete;
		rgen(rgen&& other) noexcept;
		rgen& operator=(const rgen& other) = delete;
		rgen& operator=(rgen&& other) noexcept;
		~rgen() = default;
		
		friend bool operator==(const rgen& lhs, const rgen& rhs) noexcept;
		friend bool operator!=(const rgen& lhs, const rgen& rhs) noexcept;
		friend struct std::hash<rgen>;
		explicit operator bool() const { return good(); }
		bool operator!() const { return !good(); }

		[[nodiscard]] binary_op random_op(binary_op_category category) const;
		[[nodiscard]] std::vector<std::uint8_t> generate(size_t bytes) const;
		[[nodiscard]] int generate_shift_param(int binary_digits) const;
	private:
		explicit rgen(bool) noexcept;
		std::unique_ptr<internal::rgen_impl> m_gen;
	};


	template<concepts::up_to_ui128 UnsignedInteger>
	uint128_t create_random_in_range(const rgen& rgen)
	{
		assert(rgen.good());
		using byte_array = typename uint128_t::byte_array;
		auto vec = rgen.generate(sizeof(UnsignedInteger));
		auto arr = byte_array{};
		assert(vec.size() <= arr.size());
		std::copy(vec.cbegin(), vec.cend(), arr.begin());
		return uint128_t::make_from_bytes_little_endian(arr);
	}

	template <concepts::up_to_ui128 UnsignedInteger>
	int create_shift_param_for_type(const rgen& rgen) 
	{
		return rgen.generate_shift_param(std::numeric_limits<UnsignedInteger>::digits);
	}



	//static_assert(std::is_same_v<twister_t, std::mt19937_64>);

	//inline std::mt19937_64 init_twister()
	//{
	//	std::random_device rd;
	//	auto seed = rd();
	//	return std::mt19937_64{ seed };
	//}

}

#endif
