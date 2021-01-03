#include "int128_test_gen.hpp"

namespace cjm::uint128_tests::generator
{
	using twister_t = std::conditional_t<sizeof(std::uintptr_t) == 8, std::mt19937_64, std::mt19937>;
	using twister_seed_t = std::conditional_t<sizeof(std::uintptr_t) == 8, typename std::mt19937_64::result_type, typename std::mt19937::result_type>;

	[[nodiscard]] uint128_t unsafe_create_in_range(const rgen& rgen, size_t size);
	
	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		unsafe_create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		unsafe_create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);
}
namespace cjm::uint128_tests::generator::internal
{
	class rgen_impl final
	{
	public:
		static std::unique_ptr<rgen_impl> make_rgen();
		~rgen_impl() = default;
		rgen_impl(const rgen_impl& other) = delete;
		rgen_impl(rgen_impl&& other) noexcept = delete;
		rgen_impl& operator=(const rgen_impl& other) = delete;
		rgen_impl& operator=(rgen_impl&& other) noexcept = delete;

		std::vector<std::uint8_t> generate(size_t bytes);
		int generate_shift_param(int num_digits);
		template<typename Int> requires(std::integral<Int> &&
			!cjm::numerics::concepts::character<Int> && (!std::is_same_v<std::remove_const_t<Int>, std::uint8_t>))
			Int random_from_range(const std::uniform_int_distribution<Int>& distrib);
	private:
		static twister_t init_twister();
		rgen_impl();
		twister_t m_twister;

	};

	template <typename Int>  requires(std::integral<Int> &&
		!cjm::numerics::concepts::character<Int> && (!std::is_same_v<std::remove_const_t<Int>, std::uint8_t>))
	Int rgen_impl::random_from_range(const std::uniform_int_distribution<Int>& distrib)
	{
		return distrib(m_twister);
	}
}

namespace
{
	using ssize_t = std::make_signed_t<size_t>;
	 
}

std::unique_ptr<cjm::uint128_tests::generator::internal::rgen_impl> cjm::uint128_tests::generator::internal::rgen_impl::make_rgen()
{
	return std::unique_ptr<rgen_impl>(new rgen_impl());
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::internal::rgen_impl::generate(size_t bytes)
{
	
	auto distrib = std::uniform_int_distribution<std::uint64_t>();
	auto v = std::vector<uint8_t>{};
	v.reserve(bytes);
	auto diff = static_cast<ssize_t>(bytes - v.size());
	if (diff < 0) throw std::invalid_argument{ "Too many bytes ... conversion to signed causes overflow!" };
	do 
	{
		auto generated = distrib(m_twister);
		const auto* byte_ptr_begin = reinterpret_cast<unsigned char*>(&generated);
		const auto* byte_ptr_end = byte_ptr_begin + ( diff < static_cast<ssize_t>(sizeof(generated)) ? diff : sizeof(generated));
		for (const auto* it = byte_ptr_begin; it != byte_ptr_end; ++it)
		{
			v.push_back(*it);
		}
		diff = static_cast<ssize_t>(bytes - v.size());
	} while (diff > 0);
	return v;
}

int cjm::uint128_tests::generator::internal::rgen_impl::generate_shift_param(int num_digits)
{
	auto distrib = std::uniform_int_distribution<int>(0 ,num_digits-1);
	return distrib(m_twister);
}



cjm::uint128_tests::generator::twister_t cjm::uint128_tests::generator::internal::rgen_impl::init_twister()
{
		auto seed_gen = std::random_device{};
		auto seed = seed_gen();
		auto twister = twister_t{ seed };
		return twister;	
}

cjm::uint128_tests::generator::internal::rgen_impl::rgen_impl() : m_twister{init_twister()}
{
	
	
	
}


cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_modulus_op(const rgen& rgen,
		size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	return unsafe_create_modulus_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_modulus_ops(const rgen& rgen,
		size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
	if (num_ops > 0)
	{
		vector.reserve(num_ops);
		while (vector.size() < num_ops)
		{
			vector.emplace_back(unsafe_create_modulus_op(rgen, max_dividend_size, max_divisor_size));
		}
	}
	return vector;
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_compare_op(const rgen& rgen)
{
	const auto op = binary_op::compare;
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_random_in_range<uint128_t>(rgen);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_shift_op(const rgen& rgen)
{
	const auto op = rgen.random_op(binary_op_category::bitshift);
	assert(op >= first_shift_op && op <= last_shift_op);
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_shift_param_for_type<uint128_t>(rgen);
	assert(right >= 0 && right < std::numeric_limits<uint128_t>::digits);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
                                     cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_bw_op(const rgen& rgen)
{
	const auto op = rgen.random_op(binary_op_category::bitwise);
	assert(op >= first_bw_op && op <= last_bw_op);
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_random_in_range<uint128_t>(rgen);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
                                     cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_division_op(const rgen& rgen, 
                                                                                                                           size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	return unsafe_create_division_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
		cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_division_ops(const rgen& rgen, 
			size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
	if (num_ops > 0)
	{
		vector.reserve(num_ops);
		while(vector.size() < num_ops)
		{
			vector.emplace_back(unsafe_create_division_op(rgen, max_dividend_size, max_divisor_size));
		}
	}
	return vector;	
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::create_random_in_range(const rgen& rgen,
	size_t byte_limit)
{
	if (byte_limit == 0) throw std::invalid_argument{ "zero is not a valid size." };
	return unsafe_create_in_range(rgen, byte_limit);	
}

cjm::uint128_tests::generator::rgen::rgen() : m_gen()
{
	m_gen = internal::rgen_impl::make_rgen();
	assert(m_gen);
}

cjm::uint128_tests::generator::rgen::rgen(rgen&& other) noexcept : rgen(true)
{
	std::swap(m_gen, other.m_gen);
}

cjm::uint128_tests::generator::rgen& cjm::uint128_tests::generator::rgen::operator=(rgen&& other) noexcept
{
	if (other != *this)
	{
		std::swap(other.m_gen, m_gen);
	}
	return *this;
}

cjm::uint128_tests::binary_op cjm::uint128_tests::generator::rgen::
random_op(binary_op_category category) const 
{
	assert(rgen.good());
	
	if (category == binary_op_category::compare) //only one in cat.
	{
		return binary_op::compare;
	}
	
	std::uniform_int_distribution<unsigned> distrib;

	auto to_unsigned = [](binary_op op) -> unsigned { return static_cast<unsigned>(op); };
	
	switch (category)  // NOLINT(clang-diagnostic-switch-enum)
	{
	default:
	case binary_op_category::any: 
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_op), to_unsigned(last_op) };
		break;
	case binary_op_category::bitwise:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_bw_op), to_unsigned(last_bw_op)};
		break;
	case binary_op_category::bitshift: 
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_shift_op), to_unsigned(last_shift_op) };
		break;
	case binary_op_category::add_subtract_multiply: 
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_add_sub_mul_op), to_unsigned(last_add_sub_mul_op) };
		break;
	case binary_op_category::div_mod:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_divmod_op), to_unsigned(last_divmod_op) };
		break;	
	}
	assert(distrib.min() <= distrib.max());

	unsigned value = m_gen->random_from_range(distrib);
	auto ret = static_cast<binary_op>(value);
	assert(ret >= first_op && ret <= last_op);
	return ret;
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::rgen::generate(size_t bytes) const
{
	assert(good());
	return m_gen->generate(bytes);
}
[[nodiscard]] int cjm::uint128_tests::generator::rgen::generate_shift_param(int binary_digits) const
{
	if (binary_digits - 1 <= 0) throw std::invalid_argument{ "binary digits must be at least two." };
	return m_gen->generate_shift_param(binary_digits);
}
cjm::uint128_tests::generator::rgen::rgen(bool) noexcept
{
	m_gen = nullptr;
}

bool cjm::uint128_tests::generator::operator==(const rgen& lhs, const rgen& rhs) noexcept
{
	return lhs.m_gen.get() == rhs.m_gen.get();
}

bool cjm::uint128_tests::generator::operator!=(const rgen& lhs, const rgen& rhs) noexcept
{
	return !(lhs == rhs);
}

size_t std::hash<cjm::uint128_tests::generator::rgen>::operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept
{
	cjm::uint128_tests::generator::internal::rgen_impl* ptr = key.m_gen.get();
	return std::hash<cjm::uint128_tests::generator::internal::rgen_impl*>{}(ptr);
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_division_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
	assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
	const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
	uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	while (right_operand == 0_u128)
	{
		right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	}
	const auto op = binary_op::divide;
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_modulus_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
	assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
	const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
	uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	while (right_operand == 0_u128)
	{
		right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	}
	const auto op = binary_op::modulus;
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::unsafe_create_in_range(const rgen& rgen, size_t size)
{
	using byte_array = typename uint128_t::byte_array;
	assert(rgen.good());
	assert(size > 0 && size <= sizeof(uint128_t));
	auto vec = rgen.generate(size);
	auto arr = byte_array{};
	assert(vec.size() <= arr.size());
	std::copy(vec.cbegin(), vec.cend(), arr.begin());
	return uint128_t::make_from_bytes_little_endian(arr);
}