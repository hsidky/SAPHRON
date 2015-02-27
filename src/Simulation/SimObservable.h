#pragma once

#include "../Visitors/Visitable.h"
#include "SimEvent.h"
#include "SimObserver.h"
#include <list>

using namespace Visitors;

namespace Simulation
{
	// Abstract class for observables in a simulation.
	class SimObservable : public Visitable
	{
		private:
			std::list<SimObserver*> _observers;

		public:

			// Add simulation observer.
			void AddObserver(SimObserver* observer);

			// Remove simulation observer.
			void RemoveObserver(SimObserver* observer);

			// Notify registered observers of a change.
			void NotifyObservers();

			// Get the iteration count.
			virtual int GetSimIteration() = 0;
	};
}
