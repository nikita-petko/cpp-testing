#pragma once

#include <future>
#include "argument_null_exception.hpp"
#include "execution_context.hpp"

template<typename TInput, typename TOutput>
class pipeline_handler
{
private:
	pipeline_handler<TInput, TOutput>* _next;

public:
	pipeline_handler() : _next(nullptr) {}

	pipeline_handler<TInput, TOutput>* get_next_handler() const { return _next; }
	void							   set_next_handler(pipeline_handler<TInput, TOutput>* value) { _next = value; }

	virtual auto invoke(execution_context<TInput, TOutput>* context) -> void
	{
		if (context == nullptr)
		{
			throw argument_null_exception("context");
		}
		if (this->_next == nullptr)
		{
			return;
		}
		this->_next->invoke(context);
	}
	virtual auto invoke_async(execution_context<TInput, TOutput>* context) -> std::future<void>
	{
		if (context == nullptr)
		{
			throw argument_null_exception("context");
		}
		if (this->_next == nullptr)
		{
			return std::async(std::launch::async, []() {});
		}
		return this->_next->invoke_async(context);
	}
};