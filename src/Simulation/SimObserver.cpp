#include "SimObservable.h"
#include "SimObserver.h"

namespace Simulation
{
	void SimObserver::Update(SimEvent& e)
	{
		this->SetIteration(e.GetObservable()->GetSimIteration());
		_forceObserve = e.ForceObserve();
		_identifier = e.GetObservable()->GetObservableID();
	};

	// Determines of an observer is interested in observing a particular event.
	bool SimObserver::IsObservableEvent(SimEvent& e)
	{
		return e.GetObservable()->GetSimIteration() % _frequency == 0 || e.ForceObserve();
	}
}
