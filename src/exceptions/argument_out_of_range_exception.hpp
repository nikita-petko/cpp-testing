#pragma once

#include <string>
#include "argument_exception.hpp"

class argument_out_of_range_exception : public argument_exception
{
private:
	// default message
	const char* _default_message = "Specified argument was out of the range of valid values.";

	void* _actual_value;

public:
	argument_out_of_range_exception() : argument_exception(_default_message) {}

	argument_out_of_range_exception(const std::string& param_name) : argument_exception(_default_message, param_name) {}

	argument_out_of_range_exception(const std::string& param_name, const std::string& message) : argument_exception(message, param_name) {}

	argument_out_of_range_exception(const std::string& message, exception* inner_exception) : argument_exception(message, inner_exception)
	{
	}

	argument_out_of_range_exception(const std::string& param_name, auto actual_value, const std::string& message)
	: argument_exception(message, param_name)
	, _actual_value(std::addressof(actual_value))
	{
	}

	auto get_message() const noexcept -> const std::string override
	{
		if (_actual_value != nullptr)
		{
			return format("%s" NEWLINE "Actual value was %s.", _message.c_str(), _actual_value);
		}

		return _message;
	}

	auto get_actual_value() const -> void* { return _actual_value; }
};