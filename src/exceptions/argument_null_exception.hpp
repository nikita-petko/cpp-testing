#pragma once

#include <string>
#include "argument_exception.hpp"
#include "exception.hpp"

class argument_null_exception : public argument_exception
{
private:
	// default message
	static const char* const _default_message;

public:
	argument_null_exception() : argument_exception(_default_message) {}

	argument_null_exception(std::string param_name) : argument_exception(_default_message, param_name) {}

	argument_null_exception(std::string message, const exception* inner_exception) : argument_exception(message, inner_exception) {}

	argument_null_exception(std::string param_name, std::string message) : argument_exception(message, param_name) {}
};

const char* const argument_null_exception::_default_message = "Value cannot be null.";