#pragma once

#include <cmath>
#include <ctime>
#include "argument_out_of_range_exception.hpp"
#include "format.hpp"
#include "jitter.hpp"
#include "random.hpp"

class exponential_backoff
{
private:
	typedef double (*_next_random_double_func)(void);

	static const int _ceiling_for_max_attempts = 10;
	static random_impl*	 _random;

	static auto _calculate_backoff(uint8_t					attempt,
								   uint8_t					maxAttempts,
								   time_t					baseDelay,
								   time_t					maxDelay,
								   jitter					jitter,
								   _next_random_double_func nextRandomDouble) -> time_t
	{
		if (maxAttempts > _ceiling_for_max_attempts)
		{
			throw argument_out_of_range_exception("maxAttempts",
												  maxAttempts,
												  format("maxAttempts must be less than or equal to %d.", _ceiling_for_max_attempts));
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
			case jitter::full:
				delay = (time_t)(delay * random);
				break;
			case jitter::equal:
				delay = (time_t)(delay * (0.5 + random * 0.5));
				break;
		}

		return delay;
	}

public:
	static auto calculate_backoff(uint8_t attempt, uint8_t maxAttempts, time_t baseDelay, time_t maxDelay, jitter jitter = jitter::none)
		-> time_t
	{
		return _calculate_backoff(attempt, maxAttempts, baseDelay, maxDelay, jitter, []() { return _random->next_double(); });
	}
};

random_impl* exponential_backoff::_random = new random_impl();