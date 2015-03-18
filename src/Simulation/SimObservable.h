#pragma once

#include "../Visitors/Visitable.h"
#include "SimEvent.h"
#include "SimObserver.h"
#include <list>

using namespace Visitors;

namespace SAPHRON
{
	// Abstract class for observables in a simulation.
	class SimObservable : public Visitable
	{
		private:
			std::list<SimObserver*> _observers;

		public:

			// Add simulation observer.
			virtual void AddObserver(SimObserver* observer);

			// Remove simulation observer.
			virtual void RemoveObserver(SimObserver* observer);

			// Notify registered observers of a change.
			void NotifyObservers(SimEvent event);

			// Get the iteration count.
			virtual int GetSimIteration() = 0;

			// Get an identifier of the observable.
			virtual int GetObservableID() = 0;
	};
}
