#pragma once

#include <chrono>
#include <string>
#include "exception.hpp"
#include "format.hpp"

using duration = std::chrono::system_clock::duration;

class circuit_breaker_exception : public exception
{
private:
	std::string _circuit_breaker_name;

	std::chrono::system_clock::time_point* _trip_date;

public:
	circuit_breaker_exception(const std::string& circuitBreakerName, std::chrono::system_clock::time_point* tripDate)
	: _circuit_breaker_name(circuitBreakerName)
	, _trip_date(tripDate)
	{
	}

	auto get_message() const noexcept -> const std::string override
	{
		auto now	 = std::chrono::system_clock::now();
		auto tripped = _trip_date == nullptr ? now : *_trip_date;
		auto trippedTime = std::chrono::duration<double>(now - tripped).count();

		return format("CircuitBreaker Error: %s has been tripped for %.5f seconds.", _circuit_breaker_name.c_str(), trippedTime);
	}
};