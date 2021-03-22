#ifndef CJM_BOOST_UINT_COMPARE_TEST_HPP_
#define CJM_BOOST_UINT_COMPARE_TEST_HPP_
#include "fixed_uint.hpp"
#include "testing.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <utility>
#include <random>
#include <memory>
#include <vector>
#include <functional>
#include <optional>

namespace cjm::testing
{
	
	using ctrtl_u128 = boost::multiprecision::uint128_t;
	using test_u128 = numerics::uint128;
	using ctrtl_u256 = boost::multiprecision::uint256_t;
	using test_u256 = numerics::uint256_t;
	using byte = std::uint8_t;
	using arr_256_t = test_u256::byte_array;
	using arr_128_t = test_u128::byte_array;
	using test_pair_t = std::pair<ctrtl_u256, test_u256>;
	using test_pair_128_t = std::pair<ctrtl_u128, test_u128>;
	using test_pair_col_t = std::vector<test_pair_t>;
	template<typename NumberType>
	using binary_op_pair_t = std::pair<NumberType, NumberType>;
	using namespace boost::multiprecision::literals;
	using namespace numerics::uint128_literals;
	using namespace numerics::fixed_uint_literals;
	using binary_op_set_uint256_t = std::pair<binary_op_pair_t<ctrtl_u256>, binary_op_pair_t<test_u256>>;
	using binary_op_set_uint256_col_t = std::vector<binary_op_set_uint256_t>;
	enum class BinaryOperationType : unsigned short;
	template<typename TestOperandType, typename ControlOperandType>
	class BinaryOperation;
	class BinaryU256Operation;
	using binary_u256_operation_col_t = std::vector<BinaryU256Operation>;
	enum class ComparisonOperationType : unsigned short;
	template<typename TestOperandType, typename ControlOperandType>
	class ComparisonOperation;
	using ComparisonOperation256 = ComparisonOperation<test_u256, ctrtl_u256>;
	using comparison_op_set_uint256_t = std::vector<ComparisonOperation256>;
	template<typename TestOperandType, typename ControlOperandType>
	class UnaryOperation;
	using UnaryOperation256 = UnaryOperation<test_u256, ctrtl_u256>;
	using unary_op_set_uint256_t = std::vector<UnaryOperation256>;

	inline std::mt19937_64 init_twister()
	{
		std::random_device rd;
		auto seed = rd();
		return std::mt19937_64{ seed };
	}
		
	inline std::mt19937_64 Random_Engine = init_twister();
	
	test_pair_t get_pair();

	binary_u256_operation_col_t get_binary_op_u256_collection(size_t elems);

	binary_u256_operation_col_t execute_test(const binary_u256_operation_col_t& testSet);

	BinaryOperationType get_random_op_type();

	binary_op_set_uint256_t get_binary_256_op_set();

	UnaryOperation256 get_random_unary_256_op();

	unary_op_set_uint256_t get_random_unary_256_opset256(size_t numElems);

	BinaryU256Operation get_binary_op_for_u256();

	test_pair_col_t create_random_test_collection(size_t numElems);

	ComparisonOperation256 create_random_comparison_op256();

	comparison_op_set_uint256_t create_random_comparison_opset256(size_t numElems);

	test_pair_t create_random_test_pair();

	test_pair_128_t create_random_128_test_pair();

	ctrtl_u128 to_control(const test_u128& convertMe);

	ctrtl_u256 to_control(const test_u256& convertMe);

	ctrtl_u256 from_little_endian_array(const arr_256_t& source);

	ctrtl_u128 from_little_endian_array(const arr_128_t& source);

	arr_256_t to_little_endian_array(const ctrtl_u256& convertMe);

	arr_128_t to_little_endian_array(const ctrtl_u128& convertMe);

	test_u256 to_test(const ctrtl_u256& convertMe);

	test_u256 random_u256();

	test_u128 to_test(const ctrtl_u128& convertMe);

	arr_256_t random_array();

	arr_128_t random_128_array();


	namespace boost_cjm_uint_comparison
	{
		bool operator==(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator==(const test_u256& lhs, const ctrtl_u256& rhs);

		bool operator!=(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator!=(const test_u256& lhs, const ctrtl_u256& rhs);

		bool operator>(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator>(const test_u256& lhs, const ctrtl_u256& rhs);

		bool operator<(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator<(const test_u256& lhs, const ctrtl_u256& rhs);

		bool operator>=(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator>=(const test_u256& lhs, const ctrtl_u256& rhs);

		bool operator<=(const ctrtl_u256& lhs, const test_u256& rhs);

		bool operator<=(const test_u256& lhs, const ctrtl_u256& rhs);




		//////////////////////////////////////////////////////////////////////////
		bool operator==(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator==(const test_u128& lhs, const ctrtl_u128& rhs);

		bool operator!=(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator!=(const test_u128& lhs, const ctrtl_u128& rhs);

		bool operator>(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator>(const test_u128& lhs, const ctrtl_u128& rhs);

		bool operator<(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator<(const test_u128& lhs, const ctrtl_u128& rhs);

		bool operator>=(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator>=(const test_u128& lhs, const ctrtl_u128& rhs);

		bool operator<=(const ctrtl_u128& lhs, const test_u128& rhs);

		bool operator<=(const test_u128& lhs, const ctrtl_u128& rhs);

	}

	enum class BinaryOperationType : unsigned short
	{
		Addition = 0,
		Subtraction = 1,
		Multiplication = 2,
		Division = 3,
		Modulus = 4,
		Xor = 5,
		Or = 6,
		And = 7,
		LeftShift = 8,
		RightShift = 9,
	};
	template <typename TestOperandType, typename ControlOperandType>
	class BinaryOperation
	{
		
	public:
		static_assert(std::numeric_limits<TestOperandType>::is_specialized && std::numeric_limits<ControlOperandType>::is_specialized, "Types must be number-types.");
		static_assert(std::numeric_limits<TestOperandType>::digits == std::numeric_limits<ControlOperandType>::digits, "Both numbers must have same # of digits.");

		static constexpr auto MaxDefinedShift = std::numeric_limits<TestOperandType>::digits -1;

		using control_pair = std::pair<ControlOperandType, ControlOperandType>;
		using test_pair = std::pair<TestOperandType, TestOperandType>;
		using result_pair = std::optional<std::pair<std::optional<TestOperandType>, std::optional<ControlOperandType>>>;

		bool operator() ()
		{
			return is_result_successful();
		}

		BinaryOperationType operation_type() const noexcept { return m_op_type; }

		std::string operation_name() const { return m_op_name; }

		result_pair get_result_pair();

		BinaryOperation(BinaryOperationType opType, const test_pair& testPair, const control_pair& controlPair);
		BinaryOperation() = delete;
		BinaryOperation(const BinaryOperation& other) = default;
		BinaryOperation(BinaryOperation&& other) noexcept = default;
		virtual ~BinaryOperation() = default;
		BinaryOperation& operator=(const BinaryOperation& other) = default;
		BinaryOperation& operator=(BinaryOperation&& other) noexcept = default;

		friend bool operator==(const BinaryOperation& lhs, const BinaryOperation& rhs)
		{
			return lhs.m_op_type == rhs.m_op_type && lhs.m_test_pair == rhs.m_test_pair && lhs.m_control_pair == rhs.m_control_pair;
		}

		friend bool operator!=(const BinaryOperation& lhs, const BinaryOperation& rhs) { return !(lhs == rhs); }
	protected:
		virtual bool is_result_successful()
		{
			bool ret;
			const auto& temp = get_result_pair();
			assert(temp.has_value());
			const auto& actualPair = *temp;
			if (actualPair.first.has_value() != actualPair.second.has_value())
				ret = false;
			else if (actualPair.first.has_value())
			{
				const auto& testRes = *(actualPair.first);
				const auto& controlRes = *(actualPair.second);
				ret = are_equal(testRes, controlRes);
			}
			else
				ret = m_thrown_exception_control.has_value() == m_thrown_exception_test.has_value();
			return ret;
		}

		virtual bool are_equal(const TestOperandType& lhs, const ControlOperandType& rhs);

	private:
		BinaryOperationType m_op_type;
		test_pair m_test_pair;
		control_pair m_control_pair;
		std::string m_op_name;
		std::optional<std::exception> m_thrown_exception_test;
		std::optional<std::exception> m_thrown_exception_control;
		result_pair m_result_pair;
	};

	class BinaryU256Operation final : public BinaryOperation<test_u256, ctrtl_u256>
	{
	public:
		BinaryU256Operation(BinaryOperationType opType, const test_pair& testPair, const control_pair& controlPair)
			: BinaryOperation<test_u256, ctrtl_u256>(opType, testPair, controlPair) {}
	};

	enum class ComparisonOperationType : unsigned short
	{
		Equal = 0,
		NotEqual = 1,
		GreaterThan = 2,
		LessThan = 3,
		GreaterThanOrEqual = 4,
		LessThanOrEqual = 5,
	};

	template<typename TestOperandType, typename ControlOperandType>
	class ComparisonOperation
	{
	public:
		static void release_static_memory() { s_operation_lookup = nullptr; }
		static constexpr size_t NumberOfComparisonOps{ 6 };
		using test_type_t = std::pair<TestOperandType, TestOperandType>;
		using control_type_t = std::pair<ControlOperandType, ControlOperandType>;
		using result_type_t = std::pair<bool, bool>;
		using comparison_op_t = std::function<result_type_t(const test_type_t&, const control_type_t&)>;
		using result_coll_t = std::unordered_map<ComparisonOperationType, result_type_t>;
		using name_lookup_t = std::array<std::string_view, NumberOfComparisonOps>;
		using op_lookup_t = std::unordered_map<ComparisonOperationType, comparison_op_t>;
		using indiv_result = std::pair<bool, std::string>;

		test_type_t get_test_operaands() const { return m_test_pair; }
		control_type_t get_control_operands() const { return m_control_pair; }
		indiv_result operator[](ComparisonOperationType cot) const;
		const result_coll_t& all_results() const
		{
			execute_tests();
			return m_results;
		}
		bool all_pass() const
		{
			execute_tests();
			for (const auto kvp : m_results)
			{
				if (kvp.second.first != kvp.second.second)
					return false;
			}
			return true;
		}

		ComparisonOperation(const std::pair<TestOperandType, TestOperandType>& testOperands, 
			const std::pair<ControlOperandType, ControlOperandType>& controlOperands) : m_results{}, 
				m_test_pair{testOperands}, m_control_pair{controlOperands} { }
		ComparisonOperation() = delete;
		ComparisonOperation(const ComparisonOperation& other) = default;
		ComparisonOperation(ComparisonOperation&& other) noexcept = default;
		ComparisonOperation& operator=(const ComparisonOperation& other) = default;
		ComparisonOperation& operator=(ComparisonOperation&& other) noexcept = default;
		~ComparisonOperation() = default;
	private:

		void execute_tests() const
		{
			if (m_results.size() != 6)
			{
				op_lookup_t& lookup = *s_operation_lookup;
				m_results.clear();
				comparison_op_t equalsOp = lookup[ComparisonOperationType::Equal];
				comparison_op_t notEqualOp = lookup[ComparisonOperationType::NotEqual];
				comparison_op_t greaterOp = lookup[ComparisonOperationType::GreaterThan];
				comparison_op_t lessOp = lookup[ComparisonOperationType::LessThan];
				comparison_op_t greaterOrEqualOp = lookup[ComparisonOperationType::GreaterThanOrEqual];
				comparison_op_t lessThanOrEqualOp = lookup[ComparisonOperationType::LessThanOrEqual];

				m_results[ComparisonOperationType::Equal] = equalsOp(m_test_pair, m_control_pair);
				m_results[ComparisonOperationType::NotEqual] = notEqualOp(m_test_pair, m_control_pair);
				m_results[ComparisonOperationType::GreaterThan] = greaterOp(m_test_pair, m_control_pair);
				m_results[ComparisonOperationType::LessThan] = lessOp(m_test_pair, m_control_pair);
				m_results[ComparisonOperationType::GreaterThanOrEqual] = greaterOrEqualOp(m_test_pair, m_control_pair);
				m_results[ComparisonOperationType::LessThanOrEqual] = lessThanOrEqualOp(m_test_pair, m_control_pair);
			}			
		}

		static op_lookup_t init_comparison_op_lookup();
		mutable result_coll_t m_results;
		test_type_t m_test_pair;
		control_type_t m_control_pair;
		inline const static std::array<std::string_view, NumberOfComparisonOps> s_operation_names
		{ "is equal to", "is not equal to", "is greater than", "is less than",
			"is greater than or equal to", "is less than or equal to" };
		inline static std::unique_ptr<op_lookup_t> s_operation_lookup = std::make_unique<op_lookup_t>(init_comparison_op_lookup());
	};

	template<typename TestOperandType, typename ControlOperandType>
	class UnaryOperation
	{
		static_assert(std::numeric_limits<TestOperandType>::is_integer && !std::numeric_limits<TestOperandType>::is_signed && 
			std::numeric_limits<ControlOperandType>::is_integer && !std::numeric_limits<ControlOperandType>::is_signed && 
			std::numeric_limits<TestOperandType>::digits == std::numeric_limits<ControlOperandType>::digits, 
			"Operand types must be unsigned integers that are the same width.");
	public:
		using test_type_t = TestOperandType;
		using control_type_t = ControlOperandType;

		bool all_passed() const { return m_all_passed; }
		bool plus_passed() const { return m_plus_passed; }
		bool minus_passed() const { return m_minus_passed; }
		bool bitwise_neg_passed() const { return m_bitwise_not_passed; }
		bool logical_neg_passed() const { return m_logical_not_passed; }

		test_type_t test_operand() const { return m_test_operand; }
		control_type_t control_operand() const { return m_control_operand; }

		UnaryOperation() = delete;
		UnaryOperation(const test_type_t& testOperand, const control_type_t& controlOperand);
		UnaryOperation(const UnaryOperation& other) = default;
		UnaryOperation(UnaryOperation&& other) noexcept = default;
		UnaryOperation& operator=(const UnaryOperation& other) = default;
		UnaryOperation& operator=(UnaryOperation&& other) noexcept = default;
		~UnaryOperation() = default;

	private:
		bool m_all_passed;
		bool m_plus_passed;
		bool m_minus_passed;
		bool m_bitwise_not_passed;
		bool m_logical_not_passed;
		test_type_t m_test_operand;
		control_type_t m_control_operand;
		static control_type_t twos_compl(control_type_t invert_me);
	};

	template <typename TestOperandType, typename ControlOperandType>
	typename BinaryOperation<TestOperandType, ControlOperandType>::result_pair BinaryOperation<TestOperandType,
	ControlOperandType>::get_result_pair()
	{
		if (m_result_pair.has_value())
		{
			return m_result_pair.value();
		}
		std::function<ControlOperandType(control_pair)> controlFunc;
		std::function<TestOperandType(test_pair)> testFunc;
		switch (m_op_type)
		{
		case BinaryOperationType::Addition:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first + p.second;};
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first + p.second; };
			break;
		case BinaryOperationType::Subtraction:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first - p.second; };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first - p.second; };
			break;
		case BinaryOperationType::Multiplication:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first * p.second; };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first * p.second; };
			break;
		case BinaryOperationType::Division:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first / p.second; };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first / p.second; };
			break;
		case BinaryOperationType::Modulus:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first % p.second; };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first % p.second; };
			break;
		case BinaryOperationType::Or:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return ((p.first) | (p.second)); };
			testFunc = [](const test_pair& p) -> TestOperandType {return ((p.first) | (p.second)); };
			break;
		case BinaryOperationType::Xor:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return ((p.first) ^ (p.second)); };
			testFunc = [](const test_pair& p) -> TestOperandType {return ((p.first) ^ (p.second)); };
			break;
		case BinaryOperationType::And:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return ((p.first) & (p.second)); };
			testFunc = [](const test_pair& p) -> TestOperandType {return ((p.first) & (p.second)); };
			break;
		case BinaryOperationType::LeftShift:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first << static_cast<unsigned int>(p.second % (MaxDefinedShift + 1)); };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first << (p.second % (MaxDefinedShift + 1)); };
			break;
		case BinaryOperationType::RightShift:
			controlFunc = [](const control_pair& p) -> ControlOperandType { return p.first >> static_cast<unsigned int>(p.second % (MaxDefinedShift + 1)); };
			testFunc = [](const test_pair& p) -> TestOperandType {return p.first >> (p.second % (MaxDefinedShift + 1)); };
			break;
		}
		std::optional<TestOperandType> testRes;
		std::optional<ControlOperandType> controlRes;
		try
		{
			testRes = testFunc(m_test_pair);
		}
		catch (const std::exception& ex)
		{
			testRes = std::nullopt;
			m_thrown_exception_test = ex;
		}

		try
		{
			controlRes = controlFunc(m_control_pair);
		}
		catch (const std::exception& ex)
		{
			controlRes = std::nullopt;
			m_thrown_exception_control = ex;
		}

		m_result_pair.emplace(std::make_pair(testRes, controlRes));
		assert(m_result_pair.has_value());
		return m_result_pair;
	}

	template <typename TestOperandType, typename ControlOperandType>
	BinaryOperation<TestOperandType, ControlOperandType>::BinaryOperation(BinaryOperationType opType,
		const test_pair& testPair, const control_pair& controlPair) : m_op_type{opType}, m_test_pair{testPair},
																	  m_control_pair{controlPair}, m_thrown_exception_test{}, 
																	  m_thrown_exception_control{}, m_result_pair{}
	{
		
		switch (opType)
		{
		case BinaryOperationType::Addition:
			m_op_name = "Addition";
			break;
		case BinaryOperationType::Subtraction:
			m_op_name = "Subtraction";
			break;
		case BinaryOperationType::Multiplication:
			m_op_name = "Multiplication";
			break;
		case BinaryOperationType::Division:
			m_op_name = "Division";
			if (testPair.second == 0 || controlPair.second == 0)
				throw std::invalid_argument("If division is specified, it is illegal to have zero-valued divisors.");
			break;
		case BinaryOperationType::Modulus:
			m_op_name = "Modulus";
			if (testPair.second == 0 || controlPair.second == 0)
				throw std::invalid_argument("If modulus is specified, it is illegal to have zero-valued divisors.");
			break;
		case BinaryOperationType::Or:
			m_op_name = "Bitwise Or";
			break;
		case BinaryOperationType::Xor:
			m_op_name = "Bitwise Exclusive Or";
			break;
		case BinaryOperationType::And:
			m_op_name = "Bitwise And";
			break;
		case BinaryOperationType::LeftShift:
			m_op_name = "Left Shift";
			break;
		case BinaryOperationType::RightShift:
			m_op_name = "Right Shift";
			break;
		default:
			throw std::invalid_argument("The specified operation type is not supported.");
			break;
		}		
	}

	template <typename TestOperandType, typename ControlOperandType>
	bool BinaryOperation<TestOperandType, ControlOperandType>::are_equal(const TestOperandType& lhs,
	                                                                     const ControlOperandType& rhs)
	{
		using namespace boost_cjm_uint_comparison;
		return lhs == rhs;
	}

	template <typename TestOperandType, typename ControlOperandType>
	typename ComparisonOperation<TestOperandType, ControlOperandType>::indiv_result ComparisonOperation<TestOperandType,
		ControlOperandType>::operator[](ComparisonOperationType cot) const
	{
		execute_tests();
		const unsigned short& shortVal = static_cast<unsigned short>(cot);
		if (shortVal > NumberOfComparisonOps)
		{
			throw std::invalid_argument("The supplied ComparisonOperationType argument is not supported.");
		}
		auto res = m_results[cot];
		std::string name = std::string(s_operation_names[shortVal]);
		return std::make_pair(res.first == res.second, name);
	}

	template <typename TestOperandType, typename ControlOperandType>
	typename ComparisonOperation<TestOperandType, ControlOperandType>::op_lookup_t ComparisonOperation<TestOperandType,
	ControlOperandType>::init_comparison_op_lookup()
	{
		op_lookup_t ret;
		comparison_op_t equal = [](const test_type_t& test, const control_type_t& control) -> result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first == test.second;
			innerRet.second = control.first == control.second;
			return innerRet;
		};
		comparison_op_t not_equal = [](const test_type_t& test, const control_type_t& control) -> result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first != test.second;
			innerRet.second = control.first != control.second;
			return innerRet;
		};
		comparison_op_t greater = [](const test_type_t& test, const control_type_t& control) -> result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first > test.second;
			innerRet.second = control.first > control.second;
			return innerRet;
		};
		comparison_op_t lesser = [](const test_type_t& test, const control_type_t& control)->result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first < test.second;
			innerRet.second = control.first < control.second;
			return innerRet;
		};

		comparison_op_t greaterOrEqual = [](const test_type_t& test, const control_type_t& control) -> result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first >= test.second;
			innerRet.second = control.first >= control.second;
			return innerRet;
		};
		comparison_op_t lessOrEqual = [](const test_type_t& test, const control_type_t& control) -> result_type_t
		{
			result_type_t innerRet;
			innerRet.first = test.first <= test.second;
			innerRet.second = control.first <= control.second;
			return innerRet;
		};

		ret[ComparisonOperationType::Equal] = equal;
		ret[ComparisonOperationType::NotEqual] = not_equal;
		ret[ComparisonOperationType::GreaterThan] = greater;
		ret[ComparisonOperationType::LessThan] = lesser;
		ret[ComparisonOperationType::GreaterThanOrEqual] = greaterOrEqual;
		ret[ComparisonOperationType::LessThanOrEqual] = lessOrEqual;
		assert(ret.size() == NumberOfComparisonOps);
		return ret;
	}

	template <typename TestOperandType, typename ControlOperandType>
	UnaryOperation<TestOperandType, ControlOperandType>::UnaryOperation(const test_type_t& testOperand,
	                                                                    const control_type_t& controlOperand):
		m_test_operand{testOperand}, m_control_operand{controlOperand}
	{
		using namespace boost_cjm_uint_comparison;
		if (testOperand != controlOperand)
			throw std::invalid_argument("test and control operands must be equal.");
		test_type_t testUnaryPlus = +testOperand;
		control_type_t controlUnaryPlus = +controlOperand;
		test_type_t testUnaryMinus = -testOperand;
		control_type_t controlUnaryMinus = twos_compl(m_control_operand);
		test_type_t testBitwiseNegation = ~testOperand;
		control_type_t controlBitwiseNegation = ~controlOperand;
		bool testLogicalNegation = static_cast<bool>(!testOperand);
		bool controlLogicalNegation = static_cast<bool>(!controlOperand);
		m_plus_passed = testUnaryPlus == controlUnaryPlus;
		m_minus_passed = testUnaryMinus == controlUnaryMinus;
		m_bitwise_not_passed = testBitwiseNegation == controlBitwiseNegation;
		m_logical_not_passed = testLogicalNegation == controlLogicalNegation;
		m_all_passed = m_plus_passed && m_minus_passed && m_bitwise_not_passed && m_logical_not_passed;
	}

	template <typename TestOperandType, typename ControlOperandType>
	typename UnaryOperation<TestOperandType, ControlOperandType>::control_type_t UnaryOperation<TestOperandType,
	ControlOperandType>::twos_compl(control_type_t invert_me)
	{
		invert_me = ~invert_me;
		return ++invert_me;
	}
}
#endif // CJM_BOOST_UINT_COMPARE_TEST_HPP_
