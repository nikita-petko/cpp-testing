#pragma once

#include <climits>
#include <cmath>
#include <ctime>
#include "argument_null_exception.hpp"
#include "argument_out_of_range_exception.hpp"

class random
{
private:
	//
	// Private Constants
	//
	static const int _MBIG	= INT_MAX;
	static const int _MSEED = 161803398;
	static const int _MZ	= 0;

	//
	// Member Variables
	//
	int _inext;
	int _inextp;
	int _seed_array[56];

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
	random() : random((int)time(0)) {}

	random(int seed)
	{
		int ii;
		int mj, mk;

		// Initialize our seed array.
		// This algorithm comes from Numerical Recipes in C (2nd Ed.)
		int subtraction = (seed == INT_MIN) ? INT_MAX : std::abs(seed);
		mj				= _MSEED - subtraction;
		_seed_array[55] = mj;
		mk				= 1;

		for (int i = 1; i < 55; i++)
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

		for (int k = 1; k < 5; k++)
		{
			for (int i = 1; i < 56; i++)
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
		seed	= 1;
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
	virtual double sample()
	{
		// Including this division at the end gives us significantly improved
		// random number distribution.
		return (_internal_sample() * (1.0 / _MBIG));
	}

private:
	int _internal_sample()
	{
		int retVal;
		int locINext  = _inext;
		int locINextp = _inextp;

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
	**Returns: An int [0..Int32.MaxValue)
	**Arguments: None
	**Exceptions: None.
	==============================================================================*/
	virtual int next() { return _internal_sample(); }

private:
	double _get_sample_for_large_range()
	{
		// The distribution of double value returned by Sample
		// is not distributed well enough for a large range.
		// If we use Sample for a range [Int32.MinValue..Int32.MaxValue)
		// We will end up getting even numbers only.

		int result = _internal_sample();

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
	**Returns: An int [minvalue..maxvalue)
	**Arguments: minValue -- the least legal value for the Random number.
	**           maxValue -- One greater than the greatest legal return value.
	**Exceptions: None.
	==============================================================================*/
	virtual int next(int minValue, int maxValue)
	{
		if (minValue > maxValue)
		{
			throw argument_out_of_range_exception("minValue");
		}

		int64_t range = (int64_t)maxValue - minValue;
		if (range <= (int64_t)INT_MAX)
		{
			return ((int)(sample() * range) + minValue);
		}
		else
		{
			return ((int)((int64_t)(_get_sample_for_large_range() * range) + minValue));
		}
	}

	/*=====================================Next=====================================
	  **Returns: An int [0..maxValue)
	  **Arguments: maxValue -- One more than the greatest legal return value.
	  **Exceptions: None.
	  ==============================================================================*/
	virtual int next(int maxValue)
	{
		if (maxValue < 0)
		{
			throw argument_out_of_range_exception("maxValue");
		}

		return (int)(sample() * maxValue);
	}

	/*=====================================Next=====================================
	  **Returns: A double [0..1)
	  **Arguments: None
	  **Exceptions: None
	  ==============================================================================*/
	virtual double next_double() { return sample(); }

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

		for (int i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++)
		{
			buffer[i] = (uint8_t)(_internal_sample() % (int)(0x7f + 1));
		}
	}
};