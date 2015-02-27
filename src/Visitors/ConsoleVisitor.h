#pragma once 

#include "../Simulation/SimObserver.h"

using namespace Simulation;

namespace Visitors
{
	class ConsoleVisitor : public SimObserver
	{
		virtual void Visit(Ensembles::NVTEnsemble<Site>* e) override;

		virtual void Update(SimEvent& ev) override;
	};
}