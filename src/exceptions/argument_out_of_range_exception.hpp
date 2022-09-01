#pragma once

#include <inttypes.h>
#include <optional>
#include <string>
#include "argument_exception.hpp"
#include "exception.hpp"

namespace com {

class argument_out_of_range_exception : public com::argument_exception
{
private:
	typedef com::argument_exception super;

	// default message
	static const char* const _default_message;

	std::optional<int64_t> _actual_value;

public:
	argument_out_of_range_exception() : super(_default_message) {}
	argument_out_of_range_exception(std::string param_name) : super(_default_message, param_name) {}
	argument_out_of_range_exception(std::string param_name, std::string message) : super(message, param_name) {}
	argument_out_of_range_exception(std::string message, com::exception* inner_exception) : super(message, inner_exception) {}
	argument_out_of_range_exception(std::string param_name, std::optional<int64_t> actual_value, std::string message)
	: super(message, param_name)
	, _actual_value(actual_value)
	{
	}

	auto get_message() const noexcept -> const std::string override
	{
		auto message = super::get_message();

		if (_actual_value.has_value())
		{
			return com::format("%s" NEWLINE "Actual value was %" PRId64 ".", message.c_str(), _actual_value.value());
		}

		return message;
	}

	auto get_actual_value() const -> const std::optional<int64_t>& { return _actual_value; }
};

const char* const argument_out_of_range_exception::_default_message = "Specified argument was out of the range of valid values.";

}  // namespace com