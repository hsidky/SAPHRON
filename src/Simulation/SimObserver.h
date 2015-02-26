#pragma once

#include "SimEvent.h"
#include "SimVisitor.h"

namespace Simulation
{
	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public SimVisitor
	{
		public:
			// Update observer when simulation has changed.
			virtual void Update(SimEvent& ev) = 0;
	};
}
