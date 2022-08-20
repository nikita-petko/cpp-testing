#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include "circuit_breaker_base.hpp"
#include "circuit_breaker_exception.hpp"

// make sure min() and max() are not defined
#ifdef min
#	undef min
#endif
#ifdef max
#	undef max
#endif

// this is an abstract class.
// it is used to define the interface for a circuit breaker.

class execution_circuit_breaker_base : public circuit_breaker_base
{
private:
	std::chrono::system_clock::time_point _next_retry;

	std::atomic<int> _should_retry_signal;

	auto is_time_for_retry() const -> bool { return get_now() >= _next_retry; }

protected:
	virtual auto get_retry_interval() const -> const time_t = 0;

private:
	auto should_retry() -> bool
	{
		int expected = 1;
		return _should_retry_signal.compare_exchange_strong(expected, 0);
	}

	auto attempt_to_proceed() -> void
	{
		try
		{
			test();
		}
		catch (circuit_breaker_exception& e)
		{
			if (!is_time_for_retry() || !should_retry())
			{
				throw;
			}
		}
	}

protected:
	virtual auto should_trip(std::exception& e) -> bool = 0;

public:
	auto execute(const std::function<void()> action) -> void
	{
		attempt_to_proceed();

		try
		{
			action();
		}
		catch (std::exception& e)
		{
			if (should_trip(e))
			{
				_next_retry = get_now() + std::chrono::seconds(get_retry_interval());
				trip();
			}

			_should_retry_signal.exchange(0);

			throw;
		}

		_should_retry_signal.exchange(0);

		reset();
	}

	auto execute_async(const std::function<std::future<void>()> action) -> std::future<void>
	{
		attempt_to_proceed();

		try
		{
			auto future = action();
			future.wait();
		}
		catch (std::exception& e)
		{
			if (should_trip(e))
			{
				_next_retry = get_now() + std::chrono::seconds(get_retry_interval());
				trip();
			}

			_should_retry_signal.exchange(0);

			throw;
		}

		_should_retry_signal.exchange(0);

		reset();

		return std::async(std::launch::async, []() {});
	}

	auto reset() -> bool override
	{
		bool flag	= circuit_breaker_base::reset();
		_next_retry = std::chrono::system_clock::time_point::min();
		return flag;
	}
};