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
}
