#pragma once

#include <asio.hpp>
#include <chrono>
#include <functional>
#include <thread>

class service_sentinel
{
private:
	typedef time_t (*monitor_interval_getter_func)();
	typedef bool (*health_check_func)();

	monitor_interval_getter_func _monitor_interval_getter;
	health_check_func			 _health_check;

	// asio
	asio::io_context*	_io_context;
	asio::steady_timer* _monitor_timer;

	bool _is_healthly;

	auto on_timer_callback(const asio::error_code& error) -> void
	{
		if (error)
		{
			return;
		}

		try
		{
			// timer.change(-1, -1)
			_monitor_timer->expires_at(std::chrono::steady_clock::time_point::min());
			_is_healthly = _health_check();
		}
		catch (...)
		{
			_is_healthly = false;
		}

		try
		{
			// timer.change(monitor_interval, -1)
			_monitor_timer->expires_from_now(std::chrono::seconds(_monitor_interval_getter()));
			_monitor_timer->async_wait(std::bind(&service_sentinel::on_timer_callback, this, std::placeholders::_1));
		}
		catch (...)
		{
		}
	}

public:
	auto is_healthly() const noexcept -> bool { return _is_healthly; }

	service_sentinel(health_check_func&& healthChecker, monitor_interval_getter_func&& monitorIntervalGetter, bool isHealthy = true)
	{
		_health_check			 = healthChecker;
		_monitor_interval_getter = monitorIntervalGetter;
		_is_healthly			 = isHealthy;

		_io_context	   = new asio::io_context();
		_monitor_timer = new asio::steady_timer(*_io_context, std::chrono::seconds(_monitor_interval_getter()));
		_monitor_timer->async_wait(std::bind(&service_sentinel::on_timer_callback, this, std::placeholders::_1));

		std::thread([this]() { _io_context->run(); }).detach();
	}

	~service_sentinel()
	{
		delete _monitor_timer;
		delete _io_context;
	}
};