#include <stdint.h>
#include <stdio.h>
#include "circuit_breaker.hpp"
#include "circuit_breaker_exception.hpp"
#include "exception.hpp"
#include "execution_circuit_breaker.hpp"
#include "execution_plan.hpp"
#include "exponential_backoff.hpp"
#include "inttypes.h"
#include "pipeline_handler.hpp"
#include "random.hpp"

#if !defined(_WIN32)
#	include <unistd.h>
#endif

class test_handler : public pipeline_handler<int, int>
{
public:
	auto invoke(execution_context<int, int>* context) -> void override
	{
		printf("test_handler::invoke(%d)\n", context->get_input());
		pipeline_handler<int, int>::invoke(context);
	}

	auto invoke_async(execution_context<int, int>* context) -> std::future<void> override
	{
		printf("test_handler::invoke_async(%d)\n", context->get_input());
		return pipeline_handler<int, int>::invoke_async(context);
	}
};

class other_test_handler : public pipeline_handler<int, int>
{
public:
	auto invoke(execution_context<int, int>* context) -> void override
	{
		printf("other_test_handler::invoke(%d)\n", context->get_input());
		pipeline_handler<int, int>::invoke(context);
	}

	auto invoke_async(execution_context<int, int>* context) -> std::future<void> override
	{
		printf("other_test_handler::invoke_async(%d)\n", context->get_input());
		return pipeline_handler<int, int>::invoke_async(context);
	}
};

auto
sleep_for(time_t milliseconds) -> void
{
#if defined(_WIN32)
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#else
	usleep(milliseconds * 1000);
#endif
}

/**
 * @brief Main function
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @param envp Environment variables
 * @return Exit code
 */
auto
main([[maybe_unused]] int32_t argc, [[maybe_unused]] const char* argv[], [[maybe_unused]] const char* envp[]) -> int32_t
{
	execution_plan<int32_t, int32_t>* plan = new execution_plan<int32_t, int32_t>();

	auto* handler		= new test_handler();
	auto* other_handler = new other_test_handler();

	plan->append_handler(handler);
	plan->append_handler(other_handler);

	plan->execute(123);
	plan->execute_async(456).wait();

	random_impl* r = new random_impl();

	for (int32_t i = 0; i < 10; i++)
	{
		plan->execute(r->next());
		plan->execute_async(r->next()).wait();

		plan->execute(r->next(1000));
		plan->execute_async(r->next(1000)).wait();

		plan->execute(r->next(1000, 2000));
		plan->execute_async(r->next(1000, 2000)).wait();
	}

	time_t backoff = exponential_backoff::calculate_backoff(2, 10, 1000, 10000);

	printf("backoff: %" PRId64 "\n", backoff);

	circuit_breaker* cb = new circuit_breaker("cb");

	cb->trip();

	sleep_for(backoff);

	try
	{
		cb->test();
	}
	catch (circuit_breaker_exception& e)
	{
		printf("circuit breaker tripped: %s\n", e.what());
	}

	cb->reset();

	execution_circuit_breaker* ecb = new execution_circuit_breaker(
		"ecb",
		[](std::exception& e) { return true; },
		[]() -> time_t { return 1000; });

	try
	{
		ecb->execute([]() -> void { throw exception("test"); });
	}
	catch (std::exception& e)
	{
	}

	sleep_for(1500);

	try
	{
		ecb->test();
	}
	catch (circuit_breaker_exception& e)
	{
		printf("circuit breaker tripped: %s\n", e.what());
	}

	ecb->reset();

	try
	{
		throw argument_null_exception("ecb");
	}
	catch (exception& e)
	{
		printf("exception: %s\n", e.what());
	}

	delete plan;
	delete r;
	delete cb;

	return 0;
}