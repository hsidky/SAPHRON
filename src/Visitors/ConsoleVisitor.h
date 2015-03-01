#pragma once

#include "../Simulation/SimObserver.h"

using namespace Simulation;

namespace Visitors
{
	class ConsoleVisitor : public SimObserver
	{
		public:
			ConsoleVisitor(SimFlags flags, unsigned int frequency = 1) 
				: SimObserver(flags, frequency){}

			virtual void VisitInternal(Ensembles::NVTEnsemble<Site>* e) override;
			virtual void VisitInternal(Models::BaseModel* m) override;
			virtual void VisitInternal(Site* s) override;
	};
}
