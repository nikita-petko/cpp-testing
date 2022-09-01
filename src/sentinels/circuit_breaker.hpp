#pragma once

#include <string>
#include "circuit_breaker_base.hpp"

namespace com::rbx::sentinels {

class circuit_breaker : public com::rbx::sentinels::circuit_breaker_base
{
private:
	std::string _name;

public:
	circuit_breaker() = delete;
	circuit_breaker(std::string name) : _name(name) {}

	auto get_name() const -> const std::string& override { return _name; }
};

}  // namespace com::rbx::sentinels