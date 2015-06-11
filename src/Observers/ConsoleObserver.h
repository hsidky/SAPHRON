#pragma once

#include "../Simulation/SimObserver.h"

namespace SAPHRON
{
	class ConsoleObserver : public SimObserver
	{
		public:
			ConsoleObserver(SimFlags flags, unsigned int frequency = 1)
				: SimObserver(flags, frequency){}

			virtual void Visit(Ensemble* e) override;
			virtual void Visit(DOSEnsemble* e) override;
			virtual void Visit(World* w) override;
			virtual void Visit(Particle* p) override;
			virtual void Visit(MoveManager* mm) override;

	};
}
