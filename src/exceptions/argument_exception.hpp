#pragma once

#include <string>
#include "exception.hpp"
#include "format.hpp"

class argument_exception : public exception
{
private:
	// default message
	static const char* const _default_message;

	// parameter name
	std::string _parameter_name;

public:
	argument_exception() : exception(_default_message), _parameter_name("") {}

	argument_exception(std::string message) : exception(message), _parameter_name("") {}

	argument_exception(std::string message, const exception* inner_exception) noexcept
	: exception(message, inner_exception)
	, _parameter_name("")
	{
	}

	argument_exception(std::string message, std::string param_name, const exception* inner_exception)
	: exception(message, inner_exception)
	, _parameter_name(param_name)
	{
	}

	argument_exception(std::string message, std::string param_name) : exception(message), _parameter_name(param_name) {}

	auto get_message() const noexcept -> const std::string override
	{
		auto message = exception::get_message();

		// check if param name is not empty or null
		if (!_parameter_name.empty())
		{
			return format("%s" NEWLINE "Parameter name: %s", message.c_str(), _parameter_name.c_str());
		}

		return message;
	}

	virtual auto get_parameter_name() const -> const std::string& { return _parameter_name; }
};

const char* const argument_exception::_default_message = "Value does not fall within the expected range.";