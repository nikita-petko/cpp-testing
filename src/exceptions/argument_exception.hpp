#pragma once

#include <string>
#include "exception.hpp"
#include "format.hpp"

class argument_exception : public exception
{
private:
	// default message
	const char* _default_message = "Value does not fall within the expected range.";

	// parameter name
	std::string _parameter_name;

public:
	argument_exception() : exception(_default_message), _parameter_name("") {}

	argument_exception(const std::string& message) : exception(message), _parameter_name("") {}

	argument_exception(const std::string& message, exception* inner_exception) noexcept
	: exception(message, inner_exception)
	, _parameter_name("")
	{
	}

	argument_exception(const std::string& message, const std::string& param_name, exception* inner_exception)
	: exception(message, inner_exception)
	, _parameter_name(param_name)
	{
	}

	argument_exception(const std::string& message, const std::string& param_name) : exception(message), _parameter_name(param_name) {}

	auto get_message() const noexcept -> const std::string override
	{
		// check if param name is not empty or null
		if (!_parameter_name.empty())
		{
			return format("%s" NEWLINE "Parameter name: %s", _message.c_str(), _parameter_name.c_str());
		}

		return _message;
	}

	virtual auto get_parameter_name() const -> const std::string& { return _parameter_name; }
};