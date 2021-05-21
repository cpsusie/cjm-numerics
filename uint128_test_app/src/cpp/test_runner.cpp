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
#include "test_runner.hpp"
#include <thread>

namespace
{
	const cjm::testing::test_clock test_clock_v = cjm::testing::test_clock{};  // NOLINT(clang-diagnostic-exit-time-destructors)
	std::atomic<const cjm::testing::test_clock*> tc_ptr;
	using namespace std::string_view_literals;
	constexpr auto tr_status_text_lookup = std::array{ "ready"sv, "running"sv,"done"sv, "moved_from"sv };
}

namespace cjm::testing
{
	const test_clock* execute_get_test_clock() noexcept;
	
	bool operator<(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	bool operator<=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	bool operator>(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	bool operator>=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	bool operator==(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	bool operator!=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
	std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const test_result_impl& obj);
	std::size_t hash_value(const test_result_impl& obj) noexcept;
	class test_result_impl final
	{

	public:

		[[nodiscard]] std::string_view name() const noexcept { return m_test_name; }
		[[nodiscard]] test_result_status status() const noexcept { return m_status; }
		[[nodiscard]] test_stamp_t started_at() const noexcept { return m_start_stamp; }
		[[nodiscard]] test_stamp_t ended_at() const noexcept { return m_end_stamp; }

		test_result_impl(const test_result_impl& other) = default;
		test_result_impl(test_result_impl&& other) noexcept = default;
		test_result_impl& operator=(const test_result_impl& other) = default;
		test_result_impl& operator=(test_result_impl&& other) noexcept = default;
		test_result_impl() : m_status(test_result_status::ready) {}
		test_result_impl(std::string test_name,
			test_result_status result_status,
			std::optional<test_stamp_t> started_at,
			std::optional<test_stamp_t> ended_at);
		~test_result_impl();

		friend bool operator<(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
		friend bool operator<=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept { return !(rhs < lhs); }
		friend bool operator>(const test_result_impl& lhs, const test_result_impl& rhs) noexcept { return rhs < lhs; }
		friend bool operator>=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept { return !(lhs < rhs); }
		friend bool operator==(const test_result_impl& lhs, const test_result_impl& rhs) noexcept;
		friend bool operator!=(const test_result_impl& lhs, const test_result_impl& rhs) noexcept { return !(lhs == rhs); }
		friend std::ostream& operator<<(std::ostream& os, const test_result_impl& obj);
		friend std::size_t hash_value(const test_result_impl& obj) noexcept;

	private:
		friend struct std::hash<cjm::testing::test_result>;
		std::string m_test_name;
		test_result_status m_status;
		test_stamp_t m_start_stamp{};
		test_stamp_t m_end_stamp{};

	};



	//class test_runner_impl
	//{
	//public:
	//	test_runner_impl(const test_runner_impl& other) = delete;
	//	test_runner_impl(test_runner_impl&& other) noexcept = delete;
	//	test_runner_impl& operator=(const test_runner_impl& other) = delete;
	//	test_runner_impl& operator=(test_runner_impl&& other) noexcept = delete;
	//	~test_runner_impl();
	//private:
	//	test_runner_impl();
	//	std::unique_ptr<std::mutex> m_mutex;
	//};

	class test_obj_impl final
	{
	public:
		using exec_func_t = std::remove_const_t<typename test_obj::exec_func_t>;
		[[nodiscard]] test_result_status status() const noexcept { return m_status; }
		[[nodiscard]] bool good() const noexcept { return can_execute(); }
		[[nodiscard]] std::string_view test_name() const noexcept { return m_name; }
		try_exec_rest_t try_execute() noexcept;
		test_result execute();


		test_obj_impl(std::string name, exec_func_t func, const test_clock* clock)
			: m_status(test_result_status::ready), m_name{ std::move(name) }, m_exec{ func }, m_clock{ clock }
		{
			if (!m_exec)
				throw std::invalid_argument{ "No function was supplied by parameter \'func\'." };
			if (!m_clock)
				throw std::invalid_argument{ "nullptr is not a valid value for parameter \'clock\'." };
			auto strong = uint128_tests::get_dedicated();
			if (!strong)
				throw std::invalid_argument{ "Unable to access a real dedicated output." };
			m_output = strong;
		}

		test_obj_impl(const test_obj_impl& other) = delete;
		test_obj_impl(test_obj_impl&& other) noexcept = default;
		test_obj_impl& operator=(const test_obj_impl& other) = delete;
		test_obj_impl& operator=(test_obj_impl&& other) noexcept;
		~test_obj_impl() = default;
		test_obj_impl() = default;
	private:

		[[nodiscard]] bool can_execute() const noexcept;
		test_result perform_execute();
		std::optional<test_stamp_t> m_began{};
		std::optional<test_stamp_t> m_ended{};
		test_result_status m_status = test_result_status::failed;
		std::string m_name;
		exec_func_t m_exec{};
		const test_clock* m_clock = nullptr;
		std::weak_ptr<output_guard::dedicated_output> m_output;
	};

	class test_obj_queue_impl final
	{
	public:
		using exec_func_t = typename test_obj_queue::exec_func_t;
		[[nodiscard]] bool done() const;
		[[nodiscard]] size_t remaining() const;
		[[nodiscard]] size_t original_count() const;


		test_obj_queue_impl(const std::vector<std::pair<std::string_view, exec_func_t>>& name_exec_pairs, const test_clock* test_clock);
		test_obj_queue_impl(const test_obj_queue& other) = delete;
		test_obj_queue_impl(test_obj_queue&& other) noexcept = delete;
		test_obj_queue_impl& operator=(const test_obj_queue& other) = delete;
		test_obj_queue_impl& operator=(test_obj_queue&& other) noexcept = delete;
		~test_obj_queue_impl();

		std::optional<test_obj> try_pop();

	private:
		test_obj_queue_impl();
		size_t m_original_size;
		mutable std::mutex m_mutex;
		std::vector<test_obj> m_tests;
		const test_clock* const m_clock;
	};

	class test_runner_impl final
	{
	public:
		test_runner_impl(std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests,
			const test_clock* test_clock, std::shared_ptr<output_guard::dedicated_output> output_guard, size_t max_test_threads);
		[[nodiscard]] test_runner_status status() const;

		std::pair<std::vector<test_result>, std::vector<test_result>> execute_and_wait_join();

		void execute();

		std::pair<std::vector<test_result>, std::vector<test_result>> wait_join();

		std::optional<std::tuple<const test_clock*, size_t, std::vector<std::pair<std::string_view, uint128_tests::test_func_t>>>> reset();
		~test_runner_impl();
	private:
		friend class test_runner;
		test_runner_impl();
		std::tuple<const test_clock*, size_t, std::vector<std::pair<std::string_view, uint128_tests::test_func_t>>> execute_reset();

		static void thread_loop(std::stop_token token, test_runner_impl& owner, size_t thread_no);

		mutable std::mutex m_mutex;
		atomic_bool m_can_start;
		std::condition_variable m_queue_empty;
		std::condition_variable m_all_threads_done;
		atomic_bool m_all_started;
		std::shared_ptr<output_guard::dedicated_output> m_output_guard;
		std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> m_original;
		std::vector<std::jthread> m_thread_vec;
		std::vector<test_result> m_passing_results;
		std::vector<test_result> m_failing_results;
		const test_clock* m_clock;
		test_obj_queue m_queue;
		test_runner_status m_status;
		size_t m_max_threads;
		std::atomic<size_t> m_running_threads;
		std::atomic<size_t> m_started_threads;
	};


}

cjm::testing::atomic_bool::atomic_bool() : m_condition_var{}, m_mutex{}, m_condition_val{false}
{

}

bool cjm::testing::atomic_bool::load() const
{
	auto lck = std::unique_lock{m_mutex};
	return m_condition_val;
}

bool cjm::testing::atomic_bool::set_and_notify_one_if(bool hold_mutex)
{
	bool ret = false;
	if (hold_mutex)
	{
		auto lck = std::unique_lock{m_mutex};
		if (!m_condition_val)
		{
			ret = (m_condition_val = true);
			m_condition_var.notify_one();
		}
	}
	else
	{
		{
			auto lck = std::unique_lock{m_mutex};
			if (!m_condition_val)
			{
				ret = (m_condition_val = true);
			}
		}
		if (ret)
		{
			m_condition_var.notify_one();
		}
	}
	return ret;
}


bool cjm::testing::atomic_bool::set_and_notify_all_if(bool hold_mutex)
{
	bool ret = false;
	if (hold_mutex)
	{
		auto lck = std::unique_lock{m_mutex};
		if (!m_condition_val)
		{
			ret = (m_condition_val = true);
			m_condition_var.notify_all();
		}
	}
	else
	{
		{
			auto lck = std::unique_lock{m_mutex};
			if (!m_condition_val)
			{
				ret = (m_condition_val = true);
			}
		}
		if (ret)
		{
			m_condition_var.notify_all();
		}
	}
	return ret;
}

void cjm::testing::atomic_bool::wait_til_set()
{
	auto lck = std::unique_lock{m_mutex};
	if (m_condition_val)
		return;
	m_condition_var.wait(lck, [&] () -> bool { return m_condition_val;});
}

cjm::testing::test_result_impl::~test_result_impl() = default;
const cjm::testing::test_clock* cjm::testing::get_test_clock()
{
	return execute_get_test_clock();
}

std::size_t cjm::testing::hash_value(const test_result_impl& obj) noexcept
{
	std::size_t seed = 0x7C2A1D06;
	boost::hash_combine(seed, obj.m_start_stamp.time_since_epoch().count());
	boost::hash_combine(seed, obj.m_end_stamp.time_since_epoch().count());
	boost::hash_combine(seed, obj.m_status);
	boost::hash_combine(seed, obj.m_test_name);
	return seed;
}

const cjm::testing::test_clock* cjm::testing::execute_get_test_clock() noexcept
{
	const auto* ptr = tc_ptr.load();
	if (!ptr)
	{
		const test_clock* np = nullptr;
		tc_ptr.compare_exchange_strong(np, &test_clock_v);	
		ptr = tc_ptr.load();		
	}
	return ptr;
	
}

bool cjm::testing::operator<(const test_result_impl& lhs, const test_result_impl& rhs) noexcept
{
	if (lhs.m_start_stamp < rhs.m_start_stamp)
		return true;
	if (rhs.m_start_stamp < lhs.m_start_stamp)
		return false;
	if (lhs.m_end_stamp < rhs.m_end_stamp)
		return true;
	if (rhs.m_end_stamp < lhs.m_end_stamp)
		return false;
	if (lhs.m_status < rhs.m_status)
		return true;
	if (rhs.m_status < lhs.m_status)
		return false;
	return (lhs.m_test_name < rhs.m_test_name);
}

bool cjm::testing::operator==(const test_result_impl& lhs, const test_result_impl& rhs) noexcept
{
	return lhs.m_start_stamp == rhs.m_start_stamp
		&& lhs.m_end_stamp == rhs.m_end_stamp
		&& lhs.m_status == rhs.m_status
		&& lhs.m_test_name == rhs.m_test_name;

}

cjm::testing::test_result_impl::test_result_impl(std::string test_name, test_result_status result_status,
	std::optional<test_stamp_t> started_at, std::optional<test_stamp_t> ended_at)
	: m_test_name{ std::move(test_name) }, m_status{ result_status }
{
	using namespace std::string_literals;
	if (started_at == std::nullopt && m_status != test_result_status::ready)
	{
		throw bad_test_result{ result_status };
	}
	if (ended_at == std::nullopt && m_status != test_result_status::ready && m_status != test_result_status::running)
	{
		throw bad_test_result{ result_status };
	}
	if (started_at.has_value() && ended_at.has_value())
	{
		if (*ended_at < *started_at)
			throw bad_test_result{ "ended_at may not be less than started_at." };
	}
	m_start_stamp = started_at.value_or(test_stamp_t{});
	m_end_stamp = ended_at.value_or(test_stamp_t{});
}



cjm::testing::test_obj_impl& cjm::testing::test_obj_impl::operator=(test_obj_impl&& other) noexcept
{
	if (this != &other)
	{
		std::swap(m_began, other.m_began);
		std::swap(m_ended, other.m_ended);
		std::swap(m_status, other.m_status);
		std::swap(m_name, other.m_name);
		std::swap(m_exec, other.m_exec);
		std::swap(m_clock, other.m_clock);
	}
	return *this;
}

bool cjm::testing::test_obj_impl::can_execute() const noexcept
{
	return m_status == test_result_status::ready && static_cast<bool>(m_exec) && static_cast<bool>(m_clock) && !m_began.has_value() && !m_ended.has_value();
}

cjm::testing::test_result cjm::testing::test_obj_impl::perform_execute()
{
	const auto clock = m_clock;

	m_status = test_result_status::running;
	test_stamp_t ended;
	auto began = clock->get_stamp();
	m_began = began;
	auto output_mgr = cjm::uint128_tests::get_dedicated();
	assert(output_mgr->has_file_out_proxy());
	auto cout = output_mgr->cout_proxy();
	auto cerr = output_mgr->cerr_proxy();
	auto temp_fout = output_mgr->try_get_output_proxy();
	std::basic_ostream<char>& fout_proxy = temp_fout.has_value() ? *temp_fout : cout;

	cout << "Beginning execution of test [" << m_name << "]." << newl;
	if (&fout_proxy != &cout)
	{
		fout_proxy << "Beginning execution of test [" << m_name << "]." << newl;
	}

	try
	{
		m_exec(fout_proxy, cerr);
		ended = clock->get_stamp();
		m_status = test_result_status::complete;
		cout << "Test [" << m_name << "], PASSED!" << newl << newl;
		if (&fout_proxy != &cout)
		{
			fout_proxy << "Test [" << m_name << "], PASSED!" << newl << newl;
		}
	}
	catch (const cjm::testing::testing_failure& ex)
	{
		ended = clock->get_stamp();
		m_status = test_result_status::failed;
		fout_proxy << newl << "TEST [" << m_name << "] FAILED with exception: " << newl << "\texception msg: \t[" << ex.what() << "]." << newl;
		if (&fout_proxy != &cout)
		{
			cout << "Test [" << m_name << "], FAILED" << newl << newl;
		}
		cerr << newl << "TEST [" << m_name << "] FAILED with exception: " << newl << "\texception msg: \t[" << ex.what() << "]." << newl;
	}
	catch (const std::exception& ex)
	{
		ended = clock->get_stamp();
		m_status = test_result_status::failed;
		fout_proxy << newl << "TEST [" << m_name << "] FAILED with exception: " << newl << "\texception msg: \t[" << ex.what() << "]." << newl;
		cerr << newl << "TEST [" << m_name << "] FAILED with exception: " << newl << "\texception msg: \t[" << ex.what() << "]." << newl;
		m_ended = ended;
		throw;
	}
	m_ended = ended;
	assert(m_ended >= m_began);

	return test_result{ std::move(m_name), m_status,  m_began, m_ended };
}

cjm::testing::test_obj_queue_impl::~test_obj_queue_impl() = default;


std::optional<cjm::testing::test_obj> cjm::testing::test_obj_queue_impl::try_pop()
{
	auto lck = std::unique_lock{ m_mutex };
	if (m_tests.empty())
		return std::nullopt;
	auto temp = std::move(m_tests.back());
	m_tests.pop_back();
	return std::make_optional(std::move(temp)); //? weird clang warning about c++11 defect report and nrvo if not move returned
}
cjm::testing::test_obj_queue_impl::test_obj_queue_impl() :m_original_size{}, m_clock{} {}

cjm::testing::test_runner_impl::test_runner_impl(
	std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests, const test_clock * test_clock,
	std::shared_ptr<output_guard::dedicated_output> output_guard, size_t max_test_threads) : m_mutex{}, m_output_guard{
	std::move(output_guard) }, m_original{ std::move(tests) }, m_thread_vec{}, m_passing_results{}, m_failing_results{}, m_clock{
	test_clock }, m_status{ test_runner_status::ready }, m_max_threads{ max_test_threads }
{
	if (m_original.empty())
		throw std::invalid_argument{ "Need at least one test!" };
	if (!m_clock)
		throw std::invalid_argument{ "Need valid pointer to clock!" };
	if (!m_output_guard)
		throw std::invalid_argument{ "Need valid output guard!" };
	if (max_test_threads < 1)
		throw std::invalid_argument{ "Need at least one test thread!" };
	m_queue = test_obj_queue{ m_original, m_clock };
	m_thread_vec.reserve(max_test_threads);
	m_passing_results.reserve(m_queue.original_count());
}

cjm::testing::test_runner_status cjm::testing::test_runner_impl::status() const
{
	auto lck = std::unique_lock{ m_mutex };
	return m_status;
}

std::pair<std::vector<cjm::testing::test_result>, std::vector<cjm::testing::test_result>> cjm::testing::test_runner_impl
::execute_and_wait_join()
{
	execute();
	std::this_thread::yield();
	return wait_join();
}

void cjm::testing::test_runner_impl::execute()
{
	[[maybe_unused]] bool can_start_set = false;
	{
		auto lck = std::unique_lock{ m_mutex };
		if (m_status != test_runner_status::ready)
			throw std::logic_error{ "Runner must be in ready state to execute." };
		assert(m_thread_vec.empty());
	}
	size_t num_test_threads = std::max(m_queue.original_count(), m_max_threads);
	auto thread_vec = std::vector<std::jthread>{};
	thread_vec.reserve(num_test_threads);
	
	for (size_t i = 1; i <= num_test_threads; ++i)
	{
		auto jthread = std::jthread{ &thread_loop, std::ref(*this), i };
		thread_vec.emplace_back(std::move(jthread));
	}
	
	{
		auto lck = std::unique_lock{ m_mutex };
		std::swap(m_thread_vec, thread_vec);
		if (m_status != test_runner_status::ready)
			throw std::logic_error{ "Runner must be in ready state to execute." };
		assert(m_output_guard->has_file_out_proxy());
		m_status = test_runner_status::running;
		m_running_threads.store(m_thread_vec.size());
		can_start_set=  m_can_start.set_and_notify_all_if(true);
	}
	assert(can_start_set);
	{
		m_all_started.wait_til_set();
		assert(m_started_threads.load() == m_thread_vec.size());
	}
	
}

std::pair<std::vector<cjm::testing::test_result>, std::vector<cjm::testing::test_result>> cjm::testing::test_runner_impl
::wait_join()
{
	std::vector<test_result> passing;
	std::vector<test_result> failing;
	{
		auto lck = std::unique_lock{ m_mutex };
		m_all_threads_done.wait(lck);
		for (auto& thrd : m_thread_vec)
		{
			if (thrd.joinable())
				thrd.join();
		}
		m_thread_vec.clear();
		passing = std::move(m_passing_results);
		failing = std::move(m_failing_results);
		m_status = test_runner_status::done;
	}
	return std::make_pair(std::move(passing), std::move(failing));
}

std::optional<std::tuple<const cjm::testing::test_clock*, size_t, std::vector<std::pair<std::string_view, cjm::uint128_tests::test_func_t>>>> cjm::testing::test_runner_impl::reset()
{
	auto lck = std::unique_lock(m_mutex);
	switch (m_status)
	{
	case test_runner_status::ready:
		return std::nullopt;
	case test_runner_status::done:
		return execute_reset();
	default:
		throw std::invalid_argument{ "Object is either currently running or has been moved from." };
	}
}

cjm::testing::test_runner_impl::~test_runner_impl()
{
	std::vector<std::jthread> threads;
	std::shared_ptr<testing::output_guard::dedicated_output> dedicated;
	{
		auto lck = std::unique_lock{ m_mutex };
		for (auto& thrd : m_thread_vec)
		{
			thrd.request_stop();
			if (thrd.joinable())
			{
				thrd.join();
			}
			m_status = test_runner_status::moved_from;			
		}
		m_queue_empty.notify_all();
		m_all_threads_done.notify_all();
		std::swap(m_thread_vec, threads);
		std::swap(m_output_guard, dedicated);		
	}	
}

cjm::testing::test_runner_impl::test_runner_impl(): m_can_start{}, m_all_started{}, m_clock{}, m_queue{},
                                                    m_status{ test_runner_status::moved_from }, m_max_threads{ 2 }, m_running_threads{ 0 }, m_started_threads{ 0 } {}

std::tuple<const cjm::testing::test_clock*, size_t,
           std::vector<std::pair<std::string_view, cjm::uint128_tests::test_func_t>>>
		cjm::testing::test_runner_impl::execute_reset()
{
	m_status = test_runner_status::moved_from;
	return std::make_tuple(m_clock, m_max_threads, std::move(m_original));
}

void cjm::testing::test_runner_impl::thread_loop(std::stop_token token, test_runner_impl & owner, size_t thread_no)
{
	std::optional<bool> set_started = std::nullopt;
	size_t num_threads = 0;
	{
		owner.m_can_start.wait_til_set();
 		num_threads = owner.m_thread_vec.size();
	}
	size_t num_started = ++owner.m_started_threads;
	if (num_started >= num_threads)
	{
		set_started= owner.m_all_started.set_and_notify_all_if(true);
	}
	assert((!set_started.has_value()) || (*set_started));
	while (!token.stop_requested() && !owner.m_queue.done())
	{
		if (auto test_object = owner.m_queue.try_pop(); test_object)
		{
			try_exec_rest_t result = test_object->try_execute();
			auto test_result = get_result(result);
			if (test_result && test_result.has_value())
			{
				if (test_result->status() == test_result_status::complete)
				{
					auto lck = std::unique_lock{ owner.m_mutex };
					owner.m_passing_results.push_back(std::move(*test_result));
				}
				else
				{
					auto lck = std::unique_lock{ owner.m_mutex };
					owner.m_failing_results.push_back(std::move(*test_result));
				}
			}
			else
			{
				auto err_msg = get_error_msg(result);
				auto lck = std::unique_lock{ owner.m_mutex };
				auto cerr = owner.m_output_guard->cerr_proxy();
				cerr << "From thread no#: [" << thread_no << "], test [" << test_object->test_name() << "] reports following error: [" << err_msg << "]." << newl;
			}
		}
		else
		{
			auto cerr = owner.m_output_guard->cerr_proxy();
			cerr << "Thread# [" << thread_no << "] popped a std::nullopt." << newl;
		}
		if (!token.stop_requested() && !owner.m_queue.done())
			std::this_thread::yield();
	}
	
	if (owner.m_queue.done())
	{
		auto lck = std::unique_lock{ owner.m_mutex };
		owner.m_queue_empty.notify_all();
	}
	if (auto new_thread_count = --owner.m_running_threads; new_thread_count == 0)
	{
		auto lck = std::unique_lock{ owner.m_mutex };
		owner.m_all_threads_done.notify_all();
	}
}

bool cjm::testing::test_obj_queue_impl::done() const
{
	auto lck = std::unique_lock<std::mutex>{ m_mutex };
	return m_tests.empty();
}

size_t cjm::testing::test_obj_queue_impl::remaining() const
{
	auto lck = std::unique_lock<std::mutex>{ m_mutex };
	return m_tests.size();
}

size_t cjm::testing::test_obj_queue_impl::original_count() const
{
	auto lck = std::unique_lock<std::mutex>{ m_mutex };
	return m_original_size;
}

cjm::testing::test_obj_queue_impl::test_obj_queue_impl(
	const std::vector<std::pair<std::string_view, exec_func_t>>&name_exec_pairs, const test_clock * test_clock) : m_clock{ test_clock }
{
	if (name_exec_pairs.empty()) throw std::invalid_argument{ "name_exec_pairs was empty." };
	if (test_clock == nullptr) throw std::invalid_argument{ "test_clock is nullptr." };

	bool any_bad = std::any_of(name_exec_pairs.cbegin(),
		name_exec_pairs.cend(), [](const std::pair<std::string_view, exec_func_t>& pair) -> bool
		{
			return pair.first.empty() || !(pair.second);
		});
	if (any_bad)
		throw std::invalid_argument{ "One or more items in name_exec_pair has empty name or a function without a target." };
	m_tests.reserve(name_exec_pairs.size());
	std::transform(name_exec_pairs.cbegin(), name_exec_pairs.cend(), std::back_inserter(m_tests),
		[&](const std::pair<std::string_view, exec_func_t>& pair) -> test_obj
		{
			return test_obj{ std::string{pair.first}, pair.second, m_clock };
		});
	m_original_size = m_tests.size();

}

//cjm::testing::test_runner_impl::test_runner_impl()
//{
//	m_mutex = std::make_unique<std::mutex>();
//}

cjm::testing::try_exec_rest_t cjm::testing::test_obj_impl::try_execute() noexcept
{
	if (!can_execute())
		return try_exec_rest_t{ "Object is not in state where the function can be executed.  Possible reasons: a- already ran or b- is running."s };
	try
	{
		return perform_execute();
	}
	catch (const std::exception& ex)
	{
		return std::string{ ex.what() };
	}
}

cjm::testing::test_result cjm::testing::test_obj_impl::execute()
{
	if (!can_execute())
		throw bad_test_obj{};
	return perform_execute();
}

cjm::testing::test_clock::test_clock()
{
	m_mutex = std::make_unique<std::mutex>();
}

std::basic_ostream<char>& cjm::testing::operator<<(std::basic_ostream<char>&os, test_result_status tr)
{
	if (tr < test_result_status::ready || tr > test_result_status::complete)
		os << "Unknown Status"sv;
	else
		os << test_result_text.at(static_cast<size_t>(tr));
	return os;
}

std::string cjm::testing::bad_test_result::create_message(test_result_status status)
{
	using namespace std::string_literals;
	switch (status)
	{
	case test_result_status::failed:
		return "A test that failed must supply timestamps for both begin and end times.  std::nullopt not permitted."s;
	default:
	case test_result_status::ready:
		return "For an unknown reason, we are creating an exception for a test in its default state."s;
	case test_result_status::running:
		return
			"A test that is running must supply an actual start timestamp.  std::nullopt is not permitted for the start time."s;
	case test_result_status::complete:
		return
			"A test that completed must supply timestamps for both begin and end times.  std::nullopt not permitted."s;
	}
}

std::string_view cjm::testing::test_result::name() const noexcept
{
	assert(m_ptr);
	return m_ptr->name();
}


cjm::testing::test_result_status cjm::testing::test_result::status() const noexcept
{
	assert(m_ptr);
	return m_ptr->status();
}

cjm::testing::test_stamp_t cjm::testing::test_result::started_at() const noexcept
{
	assert(m_ptr);
	return m_ptr->started_at();
}

cjm::testing::test_stamp_t cjm::testing::test_result::ended_at() const noexcept
{
	assert(m_ptr);
	return m_ptr->ended_at();
}

cjm::testing::test_result::test_result()
{
	m_ptr = std::make_unique<test_result_impl>();
}

cjm::testing::test_result::test_result(std::string test_name, test_result_status result_status,
	std::optional<test_stamp_t> started_at, std::optional<test_stamp_t> ended_at)
{
	m_ptr = std::make_unique<test_result_impl>(std::move(test_name), result_status, started_at, ended_at);
}

cjm::testing::test_result::test_result(const test_result & other)
{
	assert(other.m_ptr);
	m_ptr = std::make_unique<test_result_impl>(*(other.m_ptr));
}

cjm::testing::test_result::test_result(test_result && other) noexcept
{
	assert(other.m_ptr);
	std::swap(m_ptr, other.m_ptr);
}

cjm::testing::test_result& cjm::testing::test_result::operator=(const test_result & other)
{
	assert(other.m_ptr);
	if (this->m_ptr.get() != other.m_ptr.get())
	{
		m_ptr = std::make_unique<test_result_impl>(*(other.m_ptr));
	}
	return *this;
}

cjm::testing::test_result& cjm::testing::test_result::operator=(test_result && other) noexcept
{
	assert(other.m_ptr);
	if (this->m_ptr.get() != other.m_ptr.get())
	{
		std::swap(m_ptr, other.m_ptr);
	}
	return *this;
}

cjm::testing::test_result::~test_result() = default;

size_t cjm::testing::test_result::hash_value() const noexcept
{
	assert(m_ptr);
	return testing::hash_value(*m_ptr);
}

bool cjm::testing::has_result(const try_exec_rest_t & res) noexcept { return res.index() == 0; }

std::optional<cjm::testing::test_result> cjm::testing::get_result(const try_exec_rest_t & res) noexcept
{
	auto temp = std::get_if<test_result>(&res);
	if (temp)
	{
		return *temp;
	}
	return std::nullopt;
}

std::string_view cjm::testing::get_error_msg(const try_exec_rest_t & res) noexcept
{
	const auto temp = std::get_if<std::string>(&res);
	if (temp)
		return std::string_view{ *temp };
	return ""sv;
}

cjm::testing::test_result_status cjm::testing::test_obj::status() const noexcept
{
	if (m_ptr)
		return m_ptr->status();
	return test_result_status::failed;
}

cjm::testing::try_exec_rest_t cjm::testing::test_obj::try_execute() noexcept
{
	if (!can_execute())
		return "The test cannot be executed because it is in an invalid or moved-from state."s;
	assert(m_ptr);
	return m_ptr->try_execute();
}
cjm::testing::test_result cjm::testing::test_obj::operator()()
{
	if (!can_execute())
		throw std::invalid_argument{ "The test object is not in a valid state for execution: either it has been moved from or it is currently or has already been executed." };
	assert(m_ptr);
	return m_ptr->execute();
}

std::string_view cjm::testing::test_obj::test_name() const noexcept
{
	assert(m_ptr);
	return m_ptr->test_name();
}

cjm::testing::test_obj::test_obj(std::string name, exec_func_t func, const test_clock * clock)
{
	m_ptr = std::make_unique<test_obj_impl>(std::move(name), func, clock);
}

cjm::testing::test_obj::test_obj(test_obj && other) noexcept : m_ptr()
{
	std::swap(m_ptr, other.m_ptr);
}

cjm::testing::test_obj& cjm::testing::test_obj::operator=(test_obj && other) noexcept
{
	if (other.m_ptr.get() != m_ptr.get())
	{
		std::swap(m_ptr, other.m_ptr);
	}
	return *this;
}

cjm::testing::test_obj::~test_obj() = default;
cjm::testing::test_obj::test_obj() = default;

bool cjm::testing::test_obj::can_execute() const noexcept
{
	return m_ptr != nullptr && m_ptr->good();
}

bool cjm::testing::test_obj_queue::done() const
{
	return m_ptr->done();
}

size_t cjm::testing::test_obj_queue::remaining() const
{
	return m_ptr->remaining();
}

size_t cjm::testing::test_obj_queue::original_count() const
{
	return m_ptr->original_count();
}

cjm::testing::test_obj_queue::test_obj_queue(
	const std::vector<std::pair<std::string_view, exec_func_t>>&name_exec_pairs, const test_clock * test_clock) : m_ptr{}
{
	m_ptr = std::make_unique<test_obj_queue_impl>(name_exec_pairs, test_clock);
}

cjm::testing::test_obj_queue::test_obj_queue(test_obj_queue && other) noexcept : m_ptr()
{
	std::swap(m_ptr, other.m_ptr);
}

cjm::testing::test_obj_queue& cjm::testing::test_obj_queue::operator=(test_obj_queue && other) noexcept
{
	if (this != &other)
	{
		std::swap(m_ptr, other.m_ptr);
	}
	return *this;
}

cjm::testing::test_obj_queue::~test_obj_queue() = default;
std::optional<cjm::testing::test_obj> cjm::testing::test_obj_queue::try_pop()
{
	return m_ptr->try_pop();
}


cjm::testing::test_obj_queue::test_obj_queue() : m_ptr() {}

std::basic_ostream<char>& cjm::testing::operator<<(std::basic_ostream<char>& os, test_runner_status status)
{
	os << tr_status_text_lookup.at(static_cast<unsigned>(status));
	return os;
}

cjm::testing::test_runner_status cjm::testing::test_runner::status() const
{
	if (!m_impl)
		return test_runner_status::moved_from;
	return m_impl->status();
}

cjm::testing::test_runner::test_runner(std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests,
	const test_clock* test_clock, std::shared_ptr<output_guard::dedicated_output> output_guard)
	: test_runner(std::move(tests), test_clock, std::move(output_guard), 
		std::thread::hardware_concurrency()) {}

cjm::testing::test_runner::test_runner(std::vector<std::pair<std::string_view, uint128_tests::test_func_t>> tests,
                                       const test_clock* test_clock, std::shared_ptr<output_guard::dedicated_output> output_guard, size_t max_test_threads)
{
	if (max_test_threads < 2)
		max_test_threads = 2;
	auto thread_use = std::min(max_test_threads, static_cast<size_t>(std::thread::hardware_concurrency()));
	m_impl = std::make_unique<test_runner_impl>(std::move(tests), test_clock, std::move(output_guard), thread_use);
}

cjm::testing::test_runner::~test_runner() = default;

cjm::testing::test_runner::test_runner(test_runner && other) noexcept : test_runner{}
{
	std::swap(m_impl, other.m_impl);
}

cjm::testing::test_runner& cjm::testing::test_runner::operator=(test_runner && other) noexcept
{
	if (this != &other)
	{
		std::swap(m_impl, other.m_impl);
	}
	return *this;
}

std::pair<std::vector<cjm::testing::test_result>, std::vector<cjm::testing::test_result>> cjm::testing::test_runner::
execute_and_wait_join()
{
	assert(m_impl);
	return m_impl->execute_and_wait_join();
}

void cjm::testing::test_runner::execute()
{
	assert(m_impl);
	m_impl->execute();
}

std::pair<std::vector<cjm::testing::test_result>, std::vector<cjm::testing::test_result>> cjm::testing::test_runner::
wait_join()
{
	assert(m_impl);
	return m_impl->wait_join();
}

void cjm::testing::test_runner::reset()
{
	if (!m_impl)
		throw std::logic_error{ "This object has been moved from and cannot be reset." };
	auto result = m_impl->reset();
	auto output_ptr = m_impl->m_output_guard;
	if (result.has_value())
	{//test_clock*, size_t, std::vector<std::pair<std::string_view, uint128_tests::test_func_t>>
		auto&& [clock_ptr, max_threads, thread_vec] = *result;
		m_impl = std::make_unique<test_runner_impl>(std::move(thread_vec), clock_ptr, output_ptr, max_threads);
		m_impl->m_status = test_runner_status::ready;
	}
}

cjm::testing::test_runner::test_runner() : m_impl{} {}




cjm::testing::test_stamp_t cjm::testing::test_clock::get_stamp() const
{
	auto lock = std::unique_lock<std::mutex>{ *m_mutex };
	return test_clock_t::now();
}

std::basic_ostream<char>& cjm::testing::operator<<(std::basic_ostream<char>&os, const test_result_impl & obj)
{

	os
		<< "TEST: \t[" << obj.m_test_name << "] \tStatus: \t[" << obj.m_status << "]";
	if (obj.m_status == test_result_status::running)
	{
		os << newl << "\tStarted at: \t[" << get_micro_text(obj.m_start_stamp.time_since_epoch()) << "]";
	}
	if (obj.m_status == test_result_status::complete || obj.m_status == test_result_status::failed)
	{

		os << newl << "\tStarted at: \t[" << get_micro_text(obj.m_start_stamp.time_since_epoch()) << "]";
		os << newl << "\tEnded at: \t[" << get_micro_text(obj.m_end_stamp.time_since_epoch()) << "]";
		const test_duration_t elapsed = std::chrono::duration_cast<test_duration_t>(obj.m_end_stamp.time_since_epoch())
			- std::chrono::duration_cast<test_duration_t>(obj.m_start_stamp.time_since_epoch());
		os << newl << "\tElapsed time: \t[" << get_micro_text(elapsed) << "]";
		os << newl << "\tTest " << (obj.m_status == test_result_status::complete ? "PASSED."sv : "FAILED."sv) << newl;
	}
	os << newl << "END TEST: \t[" << obj.m_test_name << "]." << newl;
	return os;
}

size_t std::hash<cjm::testing::test_result>::operator()(const cjm::testing::test_result & tr) const noexcept
{
	return tr.hash_value();
}

bool cjm::testing::operator<(const test_result & lhs, const test_result & rhs) noexcept
{
	assert(lhs.m_ptr && rhs.m_ptr);
	return *(lhs.m_ptr) < *(rhs.m_ptr);
}
bool cjm::testing::operator==(const test_result & lhs, const test_result & rhs) noexcept
{
	assert(lhs.m_ptr && rhs.m_ptr);
	return *(lhs.m_ptr) == *(rhs.m_ptr);
}

std::basic_ostream<char>& cjm::testing::operator<<(std::basic_ostream<char>&os, const test_result & obj)
{
	assert(obj.m_ptr);
	return (os << (*(obj.m_ptr)));
}

std::vector<std::pair<std::string_view, cjm::uint128_tests::test_func_t>> cjm::testing::build_test_vector(
	std::optional<std::span<std::string_view>> selected_tests, std::basic_ostream<char>&cerr)
{
	const auto& lookup = cjm::uint128_tests::get_test_func_lookup();
	std::vector<std::pair<std::string_view, cjm::uint128_tests::test_func_t>> ret;
	if (selected_tests.has_value())
	{
		if (selected_tests->empty())
		{
			cerr << "No tests provided.  An empty vector will be returned." << newl;
		}
		std::set<std::string_view, cjm::string::case_ignoring_trimmed_ordinal_less<char>>
			tests;
		std::span<std::string_view> src = *selected_tests;

		for (auto sv : src)
		{
			if (lookup.contains(sv))
			{
				[[maybe_unused]] auto added = tests.emplace(sv).first;
				assert(added != tests.end());
			}
			else
			{
				cerr << "Test named: [" << sv << "] not recognized and will be ignored." << newl;
			}
		}
		if (!tests.empty())
		{
			ret.reserve(tests.size());
			for (auto test_name : tests)
			{
				if (auto it = lookup.find(test_name); it != lookup.end())
				{
					const auto& pair = *it;
					if (pair.second)
					{
						ret.emplace_back(pair);
					}
					else
					{
						throw std::logic_error{ "A nullptr was found in the lookup instead of valid function pointer." };
					}
				}
				else
				{
					throw std::logic_error{ "It should have been found or it wouldn't be in the set!" };
				}
			}

			if (ret.size() < tests.size())
			{
				cerr << "Size of test vector (" << ret.size() << ") is less than the size dictated by number of chosen tests (" << tests.size() << ")." << newl;
			}
		}
	}
	else
	{
		ret.reserve(lookup.size());
		std::copy(lookup.begin(), lookup.end(), std::back_inserter(ret));
		assert(ret.size() == lookup.size());
	}
	return ret;
}
