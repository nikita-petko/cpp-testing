#pragma once

#include <string>
#include "argument_exception.hpp"
#include "exception.hpp"

namespace com {

class argument_null_exception : public com::argument_exception
{
private:
	typedef com::argument_exception super;

	// default message
	static const char* const _default_message;

public:
	argument_null_exception() : super(_default_message) {}
	argument_null_exception(std::string param_name) : super(_default_message, param_name) {}
	argument_null_exception(std::string param_name, std::string message) : super(message, param_name) {}
	argument_null_exception(std::string message, com::exception* inner_exception) : super(message, inner_exception) {}
};

const char* const argument_null_exception::_default_message = "Value cannot be null.";

}  // namespace com