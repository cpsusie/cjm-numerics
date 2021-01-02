#ifndef CJM_INT128_TESTS_INL_
#define CJM_INT128_TESTS_INL_
#include "int128_tests.hpp"

namespace cjm::uint128_tests
{
    template<typename TestType, typename ControlType>
        requires (test_uint_and_control_set<TestType, ControlType>)
    std::pair<uint_test_t, uint_test_t> cjm::uint128_tests::binary_operation::perform_calculate_result(const uint_test_t& lhs, const uint_test_t& rhs, binary_op op) noexcept
    {
        assert(static_cast<size_t>(op) < op_name_lookup.size());
        uint128_t ret = 0;
        switch (op)
        {
            case binary_op::left_shift:
                ret = lhs << static_cast<int>(rhs);
                break;
            case binary_op::right_shift:
                ret = lhs >> static_cast<int>(rhs);
                break;
            case binary_op::bw_and:
                ret = lhs & rhs;
                break;
            case binary_op::bw_or:
                ret = lhs | rhs;
                break;
            case binary_op::bw_xor:
                ret = lhs ^ rhs;
                break;
            case binary_op::divide:
                ret = lhs / rhs;
                break;
            case binary_op::modulus:
                ret = lhs % rhs;
                break;
            case binary_op::add:
                ret = lhs + rhs;
                break;
            case binary_op::subtract:
                ret = lhs - rhs;
                break;
            case binary_op::multiply:
                ret = lhs * rhs;
                break;
            case binary_op::compare:
                if (lhs == rhs)
                {
                    ret = 0;
                }
                else
                {
                    ret = lhs > rhs ? 1 : -1;
                }
                break;

        }
        return ret;
    }
    template<typename TestType, typename ControlType>
        requires (test_uint_and_control_set<TestType, ControlType>)
    cjm::uint128_tests::binary_operation::binary_operation() noexcept : m_op{cjm::uint128_tests::binary_op::left_shift }, m_lhs{}, m_rhs{} {}
    template<typename TestType, typename ControlType>
        requires (test_uint_and_control_set<TestType, ControlType>)
    cjm::uint128_tests::binary_operation::binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand,
                                                       bool calculate_now) : m_op{ op }, m_lhs{ first_operand }, m_rhs{ second_operand }, m_result{}
    {
        size_t op_code = static_cast<size_t>(op);
        if (op_code >= op_name_lookup.size())
            throw std::invalid_argument{ "The op code is not recognized." };
        if (calculate_now)
        {
            this->calculate_result();
        }
    }

    template<typename TestType, typename ControlType>
    requires (test_uint_and_control_set<TestType, ControlType>)
        cjm::uint128_tests::binary_operation::
        binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand)
            : binary_operation(op, first_operand, second_operand, false){}

    template<typename TestType, typename ControlType>
    requires (test_uint_and_control_set<TestType, ControlType>)
    cjm::uint128_tests::binary_operation::
    binary_operation(binary_op op, uint128_t first_operand, uint128_t second_operand, uint128_t result) :
            m_op{ op }, m_lhs{ first_operand }, m_rhs{ second_operand }, m_result{ result }
    {
        size_t op_code = static_cast<size_t>(op);
        if (op_code >= op_name_lookup.size())
            throw std::invalid_argument{ "The op code is not recognized." };
    }
}


#endif //CJM_INT128_TESTS_INL_
