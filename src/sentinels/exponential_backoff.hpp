#pragma once

#include <argument_out_of_range_exception.hpp>
#include <cmath>
#include <ctime>
#include <format.hpp>
#include <random.hpp>
#include "jitter.hpp"

#define CEILING_FOR_MAX_ATTEMPTS 10

namespace com::rbx::sentinels {

class exponential_backoff
{
private:
	typedef double (*next_random_double_func)(void);

	static com::random_impl* _random;

	static auto _calculate_backoff(uint8_t							  attempt,
								   const uint8_t&					  maxAttempts,
								   const time_t&					  baseDelay,
								   const time_t&					  maxDelay,
								   const com::rbx::sentinels::jitter& jitter,
								   next_random_double_func			  nextRandomDouble) -> const time_t
	{
		if (maxAttempts > CEILING_FOR_MAX_ATTEMPTS)
		{
			throw com::argument_out_of_range_exception(
				"maxAttempts",
				maxAttempts,
				com::format("maxAttempts must be less than or equal to %d.", CEILING_FOR_MAX_ATTEMPTS));
		}

		if (attempt > maxAttempts)
		{
			attempt = maxAttempts;
		}

		auto delay = (time_t)std::pow(2.0, (double)(attempt - 1)) * baseDelay;
		if (delay > maxDelay || delay < 0)
		{
			delay = maxDelay;
		}

		auto random = nextRandomDouble();

		switch (jitter)
		{
			case com::rbx::sentinels::jitter::full:
				delay = (time_t)(delay * random);
				break;
			case com::rbx::sentinels::jitter::equal:
				delay = (time_t)(delay * (0.5 + random * 0.5));
				break;
		}

		return delay;
	}

public:
	static auto calculate_backoff(uint8_t							 attempt,
								  const uint8_t&					 maxAttempts,
								  const time_t&						 baseDelay,
								  const time_t&						 maxDelay,
								  const com::rbx::sentinels::jitter& jitter = com::rbx::sentinels::jitter::none) -> const time_t
	{
		return _calculate_backoff(attempt, maxAttempts, baseDelay, maxDelay, jitter, []() { return _random->next_double(); });
	}
};

com::random_impl* exponential_backoff::_random = new com::random_impl();

}  // namespace com::rbx::sentinels