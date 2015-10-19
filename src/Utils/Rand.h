#pragma once

#include "../include/prng_engine.h"

class Rand
{
	private:
		sitmo::prng_engine rand;

	public:

		Rand(unsigned int i = 0)
		{
			rand.seed(i);
		}

		double doub()
		{
			return 2.32830643653869628906e-010 * int32();
		}

		unsigned int int32()
		{
			return (unsigned int) rand();
		}

		void seed(int i)
		{
			return rand.seed(i);
		}
};
