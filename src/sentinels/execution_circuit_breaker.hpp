#pragma once

#include <exception>
#include <string>
#include "execution_circuit_breaker_base.hpp"

class execution_circuit_breaker : public execution_circuit_breaker_base
{
private:
	std::string _name;

	typedef bool (*_failure_detector_func)(std::exception& e);
	typedef time_t (*_retry_interval_func)();

	_failure_detector_func _failure_detector;
	_retry_interval_func   _retry_interval;

public:
	execution_circuit_breaker(std::string name, _failure_detector_func failure_detector, _retry_interval_func retry_interval)
	: _name(name)
	, _failure_detector(failure_detector)
	, _retry_interval(retry_interval)
	{
	}

	auto get_name() const -> std::string override { return _name; }

protected:
	auto should_trip(std::exception& e) -> bool override { return _failure_detector(e); }

	auto get_retry_interval() const -> time_t override { return _retry_interval(); }
};