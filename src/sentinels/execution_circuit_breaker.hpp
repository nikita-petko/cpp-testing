#pragma once

#include <string>
#include "execution_circuit_breaker_base.hpp"

namespace com::rbx::sentinels {

class execution_circuit_breaker : public com::rbx::sentinels::execution_circuit_breaker_base
{
private:
	std::string _name;

	typedef bool (*failure_detector_func)(com::exception& e);
	typedef time_t (*retry_interval_func)();

	failure_detector_func _failure_detector;
	retry_interval_func	  _retry_interval;

public:
	execution_circuit_breaker(std::string name, failure_detector_func failure_detector, retry_interval_func retry_interval)
	: _name(name)
	, _failure_detector(failure_detector)
	, _retry_interval(retry_interval)
	{
	}

	auto get_name() const -> const std::string& override { return _name; }

protected:
	auto should_trip(com::exception& e) -> bool override { return _failure_detector(e); }

	auto get_retry_interval() const -> const time_t override { return _retry_interval(); }
};

}  // namespace com::rbx::sentinels