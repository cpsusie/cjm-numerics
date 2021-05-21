// Copyright © 2020-2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
#ifndef CJM_TESTING_TEST_RUNNER_HPP_
#define CJM_TESTING_TEST_RUNNER_HPP_
//#include "int128_tests.hpp"
#include "testing.hpp"
#include "int128_test_switches.hpp"
#include "int128_tests.hpp"
#include "output_guard.hpp"
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <sstream>
#include <string>
#include <span>
#include <vector>
#include <array>
#include <string_view>
#include <sstream>
#include <chrono>
#include <optional>
#include <exception>
#include <stdexcept>
#include <string>
#include <compare>
#include <variant>
#include <concepts>
#include <ratio>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <set>

namespace cjm::testing
{
	class test_result;
	class test_result_impl;
	class test_obj;
	class test_obj_impl;
	class test_obj_queue;
	class test_obj_queue_impl;
	class test_runner;
	class test_runner_impl;
	class test_clock;
	const test_clock* get_test_clock();
	enum class test_runner_status : unsigned;
	std::size_t hash_value(const test_result_impl& obj) noexcept;
	bool operator<(const test_result& lhs, const test_result& rhs) noexcept;
	bool operator<=(const test_result& lhs, const test_result& rhs) noexcept;
	bool operator>(const test_result& lhs, const test_result& rhs) noexcept;
	bool operator>=(const test_result& lhs, const test_result& rhs) noexcept;
	bool operator==(const test_result& lhs, const test_result& rhs) noexcept;
	bool operator!=(const test_result& lhs, const test_result& rhs) noexcept;
	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const test_result& obj);

	class atomic_bool;

	std::vector<std::pair<std::string_view, cjm::uint128_tests::test_func_t>> build_test_vector(std::optional<std::span<std::string_view>> selected_tests, std::basic_ostream<char>& cerr);
	
	namespace concepts
	{
		template<typename TRatio>
		concept ratio = std::is_nothrow_convertible_v<decltype(TRatio::num), std::intmax_t> && std::is_nothrow_convertible_v<decltype(TRatio::den), std::intmax_t>;

		using dble_microseconds_t = std::chrono::duration<double, std::micro>;

		template<typename TDuration>
		concept duration = std::is_arithmetic_v<typename TDuration::rep> && ratio<typename TDuration::period> &&
			requires (const TDuration& d)
		{
			{std::chrono::duration_cast<dble_microseconds_t>(d)} -> std::convertible_to<dble_microseconds_t>;
			{d.count()} -> std::convertible_to<typename TDuration::rep>;
	    };

		static_assert(duration<std::chrono::microseconds>);
		static_assert(ratio<std::micro>);
	}

	template<concepts::duration T>
	std::string get_micro_text(const T& dur)
	{
		const auto microseconds = std::chrono::duration_cast<concepts::dble_microseconds_t>(dur);
		std::stringstream strm;
		strm << microseconds.count() << " microseconds";
		return strm.str();
	}
}

namespace std
{
	template<>
	struct hash<cjm::testing::test_result>
	{
		[[nodiscard]] size_t operator()(const cjm::testing::test_result& tr) const noexcept;
	};
}

namespace cjm::testing
{
	using test_clock_t = std::chrono::steady_clock;
	using test_stamp_t = test_clock_t::time_point;
	using test_duration_t = std::chrono::duration<double, typename test_stamp_t::period>;
#ifdef _MSC_VER //For reasons unknown, it is warning me about literal operations without leading '_' 
#pragma warning(push) //being reserved for standard library when these literal operators
#pragma warning (disable:4455) //ARE FROM THE STANDARD LIBRARY!
#endif	
	using std::string_view_literals::operator ""sv;
	using std::string_literals::operator ""s;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	class atomic_bool final
	{
	public:

		[[nodiscard]] bool load() const;

		atomic_bool();
		~atomic_bool() = default;
		atomic_bool(const atomic_bool& other) = delete;
		atomic_bool(atomic_bool&& other) noexcept = delete;
		atomic_bool& operator=(const atomic_bool& other) = delete;
		atomic_bool& operator=(atomic_bool&& other) noexcept = delete;
		
		bool set_and_notify_one_if(bool hold_mutex);

		bool set_and_notify_all_if(bool hold_mutex);

		void wait_til_set();


	private:
		mutable std::condition_variable m_condition_var;
		mutable std::mutex m_mutex;
		bool m_condition_val;
	};

	class test_clock
	{
		
	public:			

		test_clock();
		
		test_clock(const test_clock& other) = delete;
		test_clock(test_clock&& other) noexcept = default;
		test_clock& operator=(const test_clock& other) = delete;
		test_clock& operator=(test_clock&& other) noexcept = default;
		~test_clock() = default;

		[[nodiscard]] test_stamp_t get_stamp() const;
		
	private:
		std::unique_ptr<std::mutex> m_mutex;
	};

	
	
	enum class test_result_status
	{
		ready = 0,
		failed,
		running,
		complete
	};



	[[maybe_unused]] constexpr std::array test_result_text = { "ready"sv, "failed"sv, "running"sv, "complete"sv };

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, test_result_status tr);
	
	
	class bad_test_result final : public std::invalid_argument
	{
	public:
		bad_test_result(const bad_test_result& other) = default;
		bad_test_result(bad_test_result&& other) noexcept = default;
		bad_test_result& operator=(const bad_test_result& other) = default;
		bad_test_result& operator=(bad_test_result&& other) noexcept = default;
				bad_test_result(const std::string& msg)
			: invalid_argument(msg) {}
		bad_test_result(const char* msg)
			: invalid_argument(msg) {}
		bad_test_result(test_result_status status) : bad_test_result(create_message(status)) {}
		~bad_test_result() override = default;
	private:
		static std::string create_message(test_result_status status);
	};

	//DO NOT USE AFTER MOVED FROM!  Will cause UB!
	class test_result
	{
	public:

		[[nodiscard]] std::string_view name() const noexcept;
		[[nodiscard]] test_result_status status() const noexcept;
		[[nodiscard]] test_stamp_t started_at() const noexcept;
		[[nodiscard]] test_stamp_t ended_at() const noexcept;
		
		test_result();
		test_result(std::string test_name, 
			test_result_status result_status, 
			std::optional<test_stamp_t> started_at, 
			std::optional<test_stamp_t> ended_at);
		test_result(const test_result& other);
		test_result(test_result&& other) noexcept;
		test_result& operator=(const test_result& other);
		test_result& operator=(test_result&& other) noexcept;
		~test_result();

		friend std::strong_ordering operator<=>(const test_result& lhs, const test_result& rhs) noexcept = default;
		friend bool operator<(const test_result& lhs, const test_result& rhs) noexcept;
		friend bool operator<=(const test_result& lhs, const test_result& rhs) noexcept { return !(rhs < lhs); }
		friend bool operator>(const test_result& lhs, const test_result& rhs) noexcept { return rhs < lhs; }
		friend bool operator>=(const test_result& lhs, const test_result& rhs) noexcept { return !(lhs < rhs); }
		friend bool operator==(const test_result& lhs, const test_result& rhs) noexcept;
		friend bool operator!=(const test_result& lhs, const test_result& rhs) noexcept { return !(lhs == rhs); }
		friend std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const test_result& obj);
		
	
	private:
		[[nodiscard]] size_t hash_value() const noexcept;
		friend class test_result_impl;
		friend struct std::hash<test_result>;
		std::unique_ptr<test_result_impl> m_ptr;
	};
	/*class test_runner
	{
	public:
		test_runner(test_runner&& other) noexcept = delete;
		test_runner(const test_runner& other) = delete;
		~test_runner();
		test_runner& operator=(const test_runner& other) = delete;
		test_runner& operator=(test_runner&& other) noexcept = delete;
	protected:
		test_runner();
		
	private:
		class test_runner_impl;
		std::unique_ptr<test_runner_impl> m_impl;
	};*/

	class bad_test_obj final : public std::logic_error
	{
	public:
		bad_test_obj(const std::string& msg)
			: logic_error(msg) {}

		bad_test_obj(const char* msg)
			: logic_error(msg) {}

		bad_test_obj() : bad_test_obj{ "The test object is in an invalid state for execution: it is likely already running or finished."s } {}
	};

	using try_exec_rest_t = std::variant<test_result, std::string>;

	bool has_result(const try_exec_rest_t& res) noexcept;
	std::optional<test_result> get_result(const try_exec_rest_t& res) noexcept;
	std::string_view get_error_msg(const try_exec_rest_t& res) noexcept;


	class test_obj final
	{
	public:
		using exec_func_t = cjm::uint128_tests::test_func_t;
		
		[[nodiscard]] test_result_status status() const noexcept;
		try_exec_rest_t try_execute() noexcept;
		test_result operator()();
		[[nodiscard]] std::string_view test_name() const noexcept;
		[[nodiscard]] explicit operator bool() const noexcept { return can_execute(); }
		[[nodiscard]] bool operator !() const noexcept { return !can_execute(); }

		test_obj(std::string name, exec_func_t func, const test_clock* clock);
		test_obj(const test_obj& other) = delete;
		test_obj(test_obj&& other) noexcept;
		test_obj& operator=(const test_obj& other) = delete;
		test_obj& operator=(test_obj&& other) noexcept;
		~test_obj();
	
	private:
		test_obj();
		[[nodiscard]] bool can_execute() const noexcept;
		friend class test_obj_impl;
		friend class test_obj_queue;
		std::unique_ptr<test_obj_impl> m_ptr;
	};

	class test_obj_queue
	{
	public:
		using exec_func_t = typename test_obj::exec_func_t;

		[[nodiscard]] bool done() const;
		[[nodiscard]] size_t remaining() const;
		[[nodiscard]] size_t original_count() const;
		
		
		test_obj_queue(const std::vector<std::pair<std::string_view, exec_func_t>>& name_exec_pairs, const test_clock* test_clock);
		test_obj_queue(const test_obj_queue& other) = delete;
		test_obj_queue(test_obj_queue&& other) noexcept;
		test_obj_queue& operator=(const test_obj_queue& other) = delete;
		test_obj_queue& operator=(test_obj_queue&& other) noexcept;
		~test_obj_queue();

		std::optional<test_obj> try_pop();		

	private:
		test_obj_queue();
		friend class test_obj_queue_impl;
		friend class test_runner_impl;
		std::unique_ptr<test_obj_queue_impl> m_ptr;		
	};

	
	
		
	enum class test_runner_status : unsigned
	{
		ready=0,
		running,
		done,
		moved_from
	};

	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, test_runner_status status);
	
	class test_runner final
	{
	public:

		[[nodiscard]] test_runner_status status() const;
		test_runner(std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests, 
			const test_clock* test_clock, std::shared_ptr<output_guard::dedicated_output> output_guard);
		test_runner(std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests, const test_clock* test_clock, 
			std::shared_ptr<output_guard::dedicated_output> output_guard, size_t max_test_threads);
		~test_runner();
		test_runner(test_runner&& other) noexcept;
		test_runner(const test_runner& other) = delete;
		test_runner& operator=(test_runner&& other) noexcept;
		test_runner& operator= (const test_runner& other) = delete;

		std::pair<std::vector<test_result>, std::vector<test_result>> execute_and_wait_join();

		void execute();

		std::pair<std::vector<test_result>, std::vector<test_result>> wait_join();

		void reset();
	
	private:
		friend class test_runner_impl;
		test_runner();
		std::unique_ptr<test_runner_impl> m_impl;
	};
	
	
}

#endif
