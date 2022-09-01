#pragma once

#include <string>
#include "exception.hpp"
#include "format.hpp"

namespace com {

class argument_exception : public com::exception
{
private:
	typedef com::exception super;

	// default message
	static const char* const _default_message;

	// parameter name
	std::string _parameter_name;

public:
	argument_exception() : super(_default_message), _parameter_name("") {}
	argument_exception(std::string message) : super(message), _parameter_name("") {}
	argument_exception(std::string message, std::string param_name) : super(message), _parameter_name(param_name) {}
	argument_exception(std::string message, com::exception* inner_exception) : super(message, inner_exception), _parameter_name("") {}
	argument_exception(std::string message, std::string param_name, com::exception* inner_exception)
	: super(message, inner_exception)
	, _parameter_name(param_name)
	{
	}

	auto get_message() const noexcept -> const std::string override
	{
		auto message = super::get_message();

		// check if param name is not empty or null
		if (!_parameter_name.empty())
		{
			return com::format("%s" NEWLINE "Parameter name: %s", message.c_str(), _parameter_name.c_str());
		}

		return message;
	}

	virtual auto get_parameter_name() const -> const std::string& { return _parameter_name; }
};

const char* const argument_exception::_default_message = "Value does not fall within the expected range.";

}  // namespace com