#pragma once

#include "SimEvent.h"
#include "../Visitors/Visitor.h"

using namespace Visitors;

namespace Simulation
{
	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public Visitor
	{
		public:
			// Update observer when simulation has changed.
			virtual void Update(SimEvent& ev) = 0;
	};
}
