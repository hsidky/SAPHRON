#include "SimObserver.h"
#include "SimObservable.h"

namespace Simulation
{
	void SimObserver::Update(SimEvent& e)
	{
			this->SetIteration(e.GetObservable()->GetSimIteration());
			_forceObserve = e.ForceObserve();
	};
}