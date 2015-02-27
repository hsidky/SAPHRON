#pragma once

#include "../Simulation/SimObserver.h"

using namespace Simulation;

namespace Visitors
{
	class ConsoleVisitor : public SimObserver
	{
		public:
			virtual void Update(SimEvent& ev) override;
			virtual void Visit(Ensembles::NVTEnsemble<Site>* e) override;
			virtual void Visit(Models::BaseModel* m) override;
			virtual void Visit(Site* s) override;
	};
}
