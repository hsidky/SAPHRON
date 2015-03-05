#include "SimObservable.h"

namespace Simulation
{
	// Add simulation observer.
	void SimObservable::AddObserver(SimObserver* observer)
	{
		_observers.push_back(observer);
	}

	// Remove simulation observer.
	void SimObservable::RemoveObserver(SimObserver* observer)
	{
		_observers.remove(observer);
	}

	// Notify all observers of a simulation event.
	void SimObservable::NotifyObservers(SimEvent event)
	{
		for(auto& observer : _observers)
			if(observer->IsObservableEvent(event))
			{
				observer->LockObserver();
				observer->Update(event);
				this->AcceptVisitor(*observer);
				observer->UnlockObserver();
			}
	}
}
