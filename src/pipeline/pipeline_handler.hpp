#pragma once

#include <argument_null_exception.hpp>
#include <future>
#include "execution_context.hpp"

namespace com::rbx::pipeline {

template<typename TInput, typename TOutput>
class pipeline_handler
{
private:
	com::rbx::pipeline::pipeline_handler<TInput, TOutput>* _next;

public:
	pipeline_handler() : _next(nullptr) {}

	com::rbx::pipeline::pipeline_handler<TInput, TOutput>* get_next_handler() const { return _next; }
	void set_next_handler(com::rbx::pipeline::pipeline_handler<TInput, TOutput>* value) { _next = value; }

	virtual auto invoke(com::rbx::pipeline::execution_context<TInput, TOutput>* context) -> void
	{
		if (context == nullptr)
		{
			throw com::argument_null_exception("context");
		}
		if (this->_next == nullptr)
		{
			return;
		}
		this->_next->invoke(context);
	}
	virtual auto invoke_async(com::rbx::pipeline::execution_context<TInput, TOutput>* context) -> std::future<void>
	{
		if (context == nullptr)
		{
			throw com::argument_null_exception("context");
		}
		if (this->_next == nullptr)
		{
			return std::async(std::launch::async, []() {});
		}
		return this->_next->invoke_async(context);
	}
};

}  // namespace com::rbx::pipeline