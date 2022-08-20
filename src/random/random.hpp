#pragma once

#include <climits>
#include <cmath>
#include <ctime>
#include "argument_null_exception.hpp"
#include "argument_out_of_range_exception.hpp"

/**
 * @brief This is named random_impl to avoid a name collision with the POSIX random function.
 */
class random_impl
{
private:
	//
	// Private Constants
	//
	static constexpr int32_t _MBIG	= INT_MAX;
	static constexpr int32_t _MSEED = 161803398;
	static constexpr int32_t _MZ	= 0;

	//
	// Member Variables
	//
	int32_t _inext;
	int32_t _inextp;
	int32_t _seed_array[56];

	//
	// Public Constants
	//

	//
	// Native Declarations
	//

	//
	// Constructors
	//

public:
	inline random_impl() : random_impl((int32_t)time(0)) {}

	inline random_impl(const int32_t& seed)
	{
		int32_t ii;
		int32_t mj, mk;

		// Initialize our seed array.
		// This algorithm comes from Numerical Recipes in C (2nd Ed.)
		int32_t subtraction = (seed == INT_MIN) ? INT_MAX : std::abs(seed);
		mj					= _MSEED - subtraction;
		_seed_array[55]		= mj;
		mk					= 1;

		for (int32_t i = 1; i < 55; i++)
		{
			// Apparently the range [1..55] is special (Knuth) and so we're wasting the 0'th position.
			ii				= (21 * i) % 55;
			_seed_array[ii] = mk;
			mk				= mj - mk;
			if (mk < 0)
			{
				mk += _MBIG;
			}
			mj = _seed_array[ii];
		}

		for (int32_t k = 1; k < 5; k++)
		{
			for (int32_t i = 1; i < 56; i++)
			{
				_seed_array[i] -= _seed_array[1 + (i + 30) % 55];
				if (_seed_array[i] < 0)
				{
					_seed_array[i] += _MBIG;
				}
			}
		}

		_inext	= 0;
		_inextp = 21;
	}

	//
	// Package Private Methods
	//

	/*====================================Sample====================================
	  **Action: Return a new random number [0..1) and reSeed the Seed array.
	  **Returns: A double [0..1)
	  **Arguments: None
	  **Exceptions: None
	  ==============================================================================*/
protected:
	inline constexpr virtual double sample()
	{
		// Including this division at the end gives us significantly improved
		// random number distribution.
		return (_internal_sample() * (1.0 / _MBIG));
	}

private:
	inline constexpr int32_t _internal_sample()
	{
		int32_t retVal;
		int32_t locINext  = _inext;
		int32_t locINextp = _inextp;

		if (++locINext >= 56)
		{
			locINext = 1;
		}

		if (++locINextp >= 56)
		{
			locINextp = 1;
		}

		retVal = _seed_array[locINext] - _seed_array[locINextp];

		if (retVal == _MBIG)
		{
			retVal--;
		}

		if (retVal < 0)
		{
			retVal += _MBIG;
		}

		_seed_array[locINext] = retVal;

		_inext	= locINext;
		_inextp = locINextp;

		return retVal;
	}

	//
	// Public Instance Methods
	//

public:
	/*=====================================Next=====================================
	**Returns: An int32_t [0..Int32.MaxValue)
	**Arguments: None
	**Exceptions: None.
	==============================================================================*/
	inline virtual constexpr int32_t next() { return _internal_sample(); }

private:
	inline constexpr double _get_sample_for_large_range()
	{
		// The distribution of double value returned by Sample
		// is not distributed well enough for a large range.
		// If we use Sample for a range [Int32.MinValue..Int32.MaxValue)
		// We will end up getting even numbers only.

		int32_t result = _internal_sample();

		// Note we can't use addition here. The distribution will be bad if we do that.
		bool negative = (_internal_sample() % 2 == 0) ? true : false;  // decide the sign based on second sample
		if (negative)
		{
			result = -result;
		}

		double d = result;
		d += (INT_MAX - 1);	 // get a number in range [0 .. 2 * Int32MaxValue - 1)
		d /= 2 * (uint32_t)INT_MAX - 1;
		return d;
	}

public:
	/*=====================================Next=====================================
	**Returns: An int32_t [minvalue..maxvalue)
	**Arguments: minValue -- the least legal value for the Random number.
	**           maxValue -- One greater than the greatest legal return value.
	**Exceptions: None.
	==============================================================================*/
	inline virtual constexpr int32_t next(const int32_t& minValue, const int32_t& maxValue)
	{
		if (minValue > maxValue)
		{
			throw argument_out_of_range_exception("minValue");
		}

		int64_t range = (int64_t)maxValue - minValue;
		if (range <= (int64_t)INT_MAX)
		{
			return ((int32_t)(sample() * range) + minValue);
		}
		else
		{
			return ((int32_t)((int64_t)(_get_sample_for_large_range() * range) + minValue));
		}
	}

	/*=====================================Next=====================================
	  **Returns: An int32_t [0..maxValue)
	  **Arguments: maxValue -- One more than the greatest legal return value.
	  **Exceptions: None.
	  ==============================================================================*/
	inline virtual constexpr int32_t next(const int32_t& maxValue)
	{
		if (maxValue < 0)
		{
			throw argument_out_of_range_exception("maxValue");
		}

		return (int32_t)(sample() * maxValue);
	}

	/*=====================================Next=====================================
	  **Returns: A double [0..1)
	  **Arguments: None
	  **Exceptions: None
	  ==============================================================================*/
	inline virtual constexpr double next_double() { return sample(); }

	/*==================================NextBytes===================================
	  **Action:  Fills the byte array with random bytes [0..0x7f].  The entire array is filled.
	  **Returns:Void
	  **Arugments:  buffer -- the array to be filled.
	  **Exceptions: None
	  ==============================================================================*/
	virtual void next_bytes(uint8_t buffer[])
	{
		if (buffer == nullptr)
		{
			throw argument_null_exception("buffer");
		}

		for (int32_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++)
		{
			buffer[i] = (uint8_t)(_internal_sample() % (int32_t)(0x7f + 1));
		}
	}
};