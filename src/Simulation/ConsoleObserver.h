#pragma once

#include "../Simulation/SimObserver.h"

namespace SAPHRON
{
	class ConsoleObserver : public SimObserver
	{
		public:
			ConsoleObserver(SimFlags flags, unsigned int frequency = 1)
				: SimObserver(flags, frequency){}

			virtual void VisitInternal(Ensemble* h) override;
	};
}
