#pragma once

#include <atomic>
#include <chrono>
#include <exception.hpp>
#include <string>
#include "execution_circuit_breaker_base.hpp"

namespace com::rbx::sentinels {

class threshold_execution_circuit_breaker : public com::rbx::sentinels::execution_circuit_breaker_base
{
private:
	typedef com::rbx::sentinels::threshold_execution_circuit_breaker self;

	typedef bool (*failure_detector_func)(com::exception& e);
	typedef time_t (*retry_interval_func)();
	typedef int (*exception_count_for_tripping_func)();
	typedef time_t (*exception_interval_for_tripping_func)();
	typedef std::chrono::system_clock::time_point (*utc_now_func)();

	failure_detector_func				 _failure_detector;
	retry_interval_func					 _retry_interval;
	exception_count_for_tripping_func	 _exception_count_for_tripping;
	exception_interval_for_tripping_func _exception_interval_for_tripping;
	utc_now_func						 _utc_now;

	std::string _name;

	std::atomic<int>					  _exception_count;
	std::chrono::system_clock::time_point _exception_count_interval_end;

private:
	threshold_execution_circuit_breaker(std::string							 name,
										failure_detector_func				 failure_detector,
										retry_interval_func					 retry_interval,
										exception_count_for_tripping_func	 exception_count_for_tripping,
										exception_interval_for_tripping_func exception_interval_for_tripping,
										utc_now_func						 utc_now)
	: _name(name)
	, _failure_detector(failure_detector)
	, _retry_interval(retry_interval)
	, _exception_count_for_tripping(exception_count_for_tripping)
	, _exception_interval_for_tripping(exception_interval_for_tripping)
	, _utc_now(utc_now)
	, _exception_count(0)
	, _exception_count_interval_end(_utc_now())
	{
	}

	static std::chrono::system_clock::time_point _default_utc_now() { return std::chrono::system_clock::now(); }

public:
	threshold_execution_circuit_breaker(std::string							 name,
										failure_detector_func				 failure_detector,
										retry_interval_func					 retry_interval,
										exception_count_for_tripping_func	 exception_count_for_tripping,
										exception_interval_for_tripping_func exception_interval_for_tripping)
	: self(name, failure_detector, retry_interval, exception_count_for_tripping, exception_interval_for_tripping, _default_utc_now)
	{
	}

	auto get_name() const -> const std::string& override { return _name; }

protected:
	auto get_retry_interval() const -> const time_t override { return _retry_interval(); }

	auto get_now() const -> const std::chrono::system_clock::time_point override { return _utc_now(); }

private:
	auto reset_exception_count() -> void
	{
		_exception_count.exchange(0);
		_exception_count_interval_end = _utc_now() + std::chrono::seconds(_exception_interval_for_tripping());
	}

protected:
	auto should_trip(com::exception& e) -> bool override
	{
		if (_failure_detector(e))
		{
			if (_exception_count_interval_end < _utc_now())
			{
				reset_exception_count();
			}
			_exception_count++;
			if (_exception_count >= _exception_count_for_tripping())
			{
				return true;
			}
		}

		return false;
	}
};

}  // namespace com::rbx::sentinels