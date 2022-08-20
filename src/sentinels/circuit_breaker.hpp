#pragma once

#include <string>
#include "circuit_breaker_base.hpp"

class circuit_breaker : public circuit_breaker_base
{
private:
	const std::string& _name;

public:
	circuit_breaker() = delete;
	circuit_breaker(const std::string& name) : _name(name) {}

	auto get_name() const -> const std::string& override { return _name; }
};