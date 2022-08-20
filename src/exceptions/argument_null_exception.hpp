#pragma once

#include <string>
#include "argument_exception.hpp"
#include "exception.hpp"

class argument_null_exception : public argument_exception
{
private:
	// default message
	const char* _default_message = "Value cannot be null.";

public:
	argument_null_exception() : argument_exception(_default_message) {}

	argument_null_exception(const std::string& param_name) : argument_exception(_default_message, param_name) {}

	argument_null_exception(const std::string& message, exception* inner_exception) : argument_exception(message, inner_exception) {}

	argument_null_exception(const std::string& param_name, const std::string& message) : argument_exception(message, param_name) {}
};