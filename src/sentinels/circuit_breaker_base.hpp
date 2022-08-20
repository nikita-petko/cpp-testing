#pragma once

#include <atomic>
#include <chrono>
#include "circuit_breaker_exception.hpp"

// this is an abstract class.
// it is used to define the interface for a circuit breaker.

class circuit_breaker_base
{
private:
	std::chrono::system_clock::time_point* _trip_date = nullptr;

protected:
	virtual auto get_now() const -> const std::chrono::system_clock::time_point { return std::chrono::system_clock::now(); }

private:
	std::atomic<int> _is_tripped;

public:
	// the name of the circuit breaker
	virtual auto get_name() const -> const std::string& = 0;

	// the date when the circuit breaker was tripped
	auto get_trip_date() const -> std::chrono::system_clock::time_point* { return _trip_date; }

	// is the circuit breaker tripped?
	auto is_tripped() const -> bool { return _is_tripped.load() == 1; }

	// reset the circuit breaker
	virtual auto reset() -> bool
	{
		bool flag = _is_tripped.exchange(0) == 1;
		if (flag)
		{
			_trip_date = nullptr;
		}

		return flag;
	}

	// test the circuit breaker and throw if it is tripped
	virtual auto test() -> void
	{
		if (is_tripped())
		{
			throw circuit_breaker_exception(get_name(), _trip_date);
		}
	}

	// trip the circuit breaker
	virtual auto trip() -> bool
	{
		bool flag = _is_tripped.exchange(1) == 1;
		if (!flag)
		{
			auto now   = get_now();
			_trip_date = &now;
		}

		return flag;
	}
};