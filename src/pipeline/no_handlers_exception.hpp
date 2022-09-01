#pragma once

#include <exception.hpp>

namespace com::rbx::pipeline {

class no_handlers_exception : public com::exception
{
};

}  // namespace com::rbx::pipeline