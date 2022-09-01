#pragma once

#include <argument_exception.hpp>
#include <argument_null_exception.hpp>
#include <format.hpp>
#include <future>
#include <vector>
#include "no_handlers_exception.hpp"
#include "pipeline_handler.hpp"

namespace com::rbx::pipeline {

template<typename TInput, typename TOutput>
class execution_plan
{
private:
	std::vector<com::rbx::pipeline::pipeline_handler<TInput, TOutput>*> _handlers;

	template<class T>
	auto _get_handler_index() -> int
	{
		for (int i = 0; i < _handlers.size(); i++)
		{
			if (typeid(T) == typeid(*_handlers[i]))
			{
				return i;
			}
		}

		return -1;
	}

public:
	execution_plan() : _handlers(std::vector<com::rbx::pipeline::pipeline_handler<TInput, TOutput>*>()) {}

	const com::rbx::pipeline::pipeline_handler<TInput, TOutput>* const* get_handlers() const noexcept { return _handlers.data(); }
	const int															get_handler_count() const noexcept { return _handlers.size(); }

	auto remove_handler(const int& index) -> void
	{
		if (index >= _handlers.size() || index < 0)
		{
			throw com::argument_exception("index does not exist in handlers.", "index");
		}

		com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler = _handlers[index];
		if (index > 0)
		{
			_handlers[index - 1]->set_next_handler(handler->get_next_handler());
		}

		_handlers.erase(_handlers.begin() + index);
	}
	template<class T>
	auto remove_handler() -> void
	{
		int index = _get_handler_index<T>();
		if (index < 0)
		{
			throw com::argument_exception(com::format("No handler of type %s was found.", typeid(T).name()), "T");
		}
		remove_handler(index);
	}

	auto append_handler(com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler) -> void
	{
		if (handler == nullptr)
		{
			throw com::argument_null_exception("handler");
		}

		insert_handler(_handlers.size(), handler);
	}
	auto prepend_handler(com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler) -> void
	{
		if (handler == nullptr)
		{
			throw com::argument_null_exception("handler");
		}

		insert_handler(0, handler);
	}

	template<class T>
	auto add_handler_after(com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler) -> void
	{
		if (handler == nullptr)
		{
			throw com::argument_null_exception("handler");
		}

		int index = _get_handler_index<T>();
		if (index < 0)
		{
			throw com::argument_exception(com::format("No handler of type %s was found.", typeid(T).name()), "T");
		}

		insert_handler(index + 1, handler);
	}
	template<class T>
	auto add_handler_before(com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler) -> void
	{
		if (handler == nullptr)
		{
			throw com::argument_null_exception("handler");
		}

		int index = _get_handler_index<T>();
		if (index < 0)
		{
			throw com::argument_exception(format("No handler of type %s was found.", typeid(T).name()), "T");
		}

		insert_handler(index, handler);
	}

	auto insert_handler(const int& index, com::rbx::pipeline::pipeline_handler<TInput, TOutput>* handler) -> void
	{
		if (index < 0 || index > _handlers.size())
		{
			throw com::argument_exception(
				"index is not valid to add to in handlers. Index must be between 0, and the current handlers count.",
				"index");
		}

		if (handler == nullptr)
		{
			throw com::argument_null_exception("handler");
		}

		for (int i = 0; i < _handlers.size(); i++)
		{
			if (_handlers[i] == handler)
			{
				throw com::argument_exception(
					"handler is already part of the execution plan. The same instance may only be used in one execution plan once at a "
					"time.",
					"handler");
			}
		}

		if (index > 0)
		{
			_handlers[index - 1]->set_next_handler(handler);
		}

		if (index < _handlers.size())
		{
			handler->set_next_handler(_handlers[index]);
		}

		_handlers.insert(_handlers.begin() + index, handler);
	}

	auto clear_handlers() -> void { _handlers.clear(); }

	auto execute(TInput input) -> TOutput
	{
		if (_handlers.size() == 0)
		{
			throw com::rbx::pipeline::no_handlers_exception();
		}

		com::rbx::pipeline::execution_context<TInput, TOutput> context(input);
		_handlers.front()->invoke(&context);
		return context.get_output();
	}

	auto execute_async(TInput input) -> std::future<TOutput>
	{
		if (_handlers.size() == 0)
		{
			throw com::rbx::pipeline::no_handlers_exception();
		}

		com::rbx::pipeline::execution_context<TInput, TOutput> context(input);
		std::shared_future<void>							   future = _handlers.front()->invoke_async(&context);
		return std::async(std::launch::async, [context, future]() -> TOutput {
			future.wait();
			return context.get_output();
		});
	}
};

}  // namespace com::rbx::pipeline