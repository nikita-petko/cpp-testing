#pragma once

#include <boost/stacktrace.hpp>
#include <boost/type_index.hpp>
#include <exception>
#include <format.hpp>
#include <string>

#ifdef _WIN32
#	define NEWLINE "\r\n"
#elif defined macintosh	 // OS 9
#	define NEWLINE "\r"
#else
#	define NEWLINE "\n"  // Mac OS X uses \n
#endif

namespace com {

class exception : public std::exception
{
private:
	com::exception*				   _inner_exception = nullptr;
	boost::stacktrace::stacktrace* _stacktrace;

protected:
	std::string _message;

	auto _construct_message() const noexcept -> std::string
	{
		auto message	= get_message();
		auto stacktrace = *_stacktrace;
		auto stack		= boost::stacktrace::to_string(stacktrace);
		auto this_type	= boost::typeindex::type_id_runtime(*this).pretty_name();

#ifdef _WIN32
		// Windows case here for adding "class " to start of typeid.
		this_type.erase(0, 6);
#endif

		// check if there is an inner exception
		if (_inner_exception != nullptr)
		{
			auto inner_message = _inner_exception->what();

			// like this:
			return com::format("%s: %s" NEWLINE "%s ------------> " NEWLINE "%s",
							   this_type.c_str(),
							   message.c_str(),
							   inner_message,
							   stack.c_str());
		}
		else
		{
			return com::format("%s: %s" NEWLINE "%s", this_type.c_str(), message.c_str(), stack.c_str());
		}
	}

public:
	exception()
	: _message(format("Exception of type '%s' was thrown.", typeid(*this).name()))
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	, _inner_exception(nullptr)
	{
	}
	exception(std::string message)
	: _message(message)
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	, _inner_exception(nullptr)
	{
	}
	exception(const std::exception& e)
	: _message(e.what())
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	, _inner_exception(nullptr)
	{
	}
	exception(std::string message, com::exception* inner_exception)
	: _message(message)
	, _stacktrace(new boost::stacktrace::stacktrace(2, static_cast<size_t>(-1)))
	, _inner_exception(inner_exception)
	{
	}

	[[nodiscard]] auto what() const noexcept -> const char* override
	{
		auto  message = _construct_message();
		char* ptr	  = new char[message.length() + 1];
		strcpy(ptr, message.c_str());

		return ptr;
	}
	virtual auto get_message() const noexcept -> const std::string { return _message; }
	auto		 get_stacktrace() const noexcept -> const boost::stacktrace::stacktrace* { return _stacktrace; }
	auto		 get_inner_exception() const noexcept -> const com::exception* { return _inner_exception; }
};

}  // namespace com