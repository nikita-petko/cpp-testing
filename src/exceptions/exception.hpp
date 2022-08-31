#pragma once

#include <boost/stacktrace.hpp>
#include <exception>
#include <memory>
#include <string>
#include "format.hpp"

#ifdef _WIN32
#	define NEWLINE "\r\n"
#elif defined macintosh	 // OS 9
#	define NEWLINE "\r"
#else
#	define NEWLINE "\n"  // Mac OS X uses \n
#endif

class exception : public std::exception
{
private:
	std::shared_ptr<const exception> _inner_exception;
	boost::stacktrace::stacktrace*	 _stacktrace;

protected:
	std::string _message;

	auto _construct_message() const noexcept -> std::string
	{
		auto message	= get_message();
		auto stacktrace = *_stacktrace;
		auto stack		= boost::stacktrace::to_string(stacktrace);

		// check if there is an inner exception
		if (_inner_exception)
		{
			auto inner_message = _inner_exception->what();

			// like this:
			return format("%s ---> " NEWLINE "%s ---> " NEWLINE "%s", message.c_str(), inner_message, stack.c_str());
		}
		else
		{
			return format("%s" NEWLINE "%s", message.c_str(), stack.c_str());
		}
	}

public:
	exception()
	: _inner_exception(NULL)
	, _message(format("Exception of type '%s' was thrown.", typeid(*this).name()))
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	{
	}

	exception(const std::exception& e)
	: _inner_exception(NULL)
	, _message(e.what())
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	{
	}
	exception(std::string message)
	: _inner_exception(NULL)
	, _message(message)
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	{
	}
	exception(std::string message, const exception* inner_exception)
	: _inner_exception(inner_exception)
	, _message(message)
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	{
	}

	virtual auto get_message() const noexcept -> const std::string { return _message; }
	auto		 get_inner_exception() const noexcept -> const std::shared_ptr<const exception> { return _inner_exception; }
	auto		 get_stacktrace() const noexcept -> const boost::stacktrace::stacktrace* { return _stacktrace; }
	auto		 what() const noexcept -> const char* override { return _construct_message().c_str(); }
};