#include "SimObservable.h"
#include "SimObserver.h"

namespace SAPHRON
{
	void SimObserver::Update(SimEvent& e)
	{
		// Only lock and proceed if we have to.
		if(e.GetIteration() % _frequency == 0 || e.ForceObserve())
		{
			_mutex.lock();
			_event = e;
			PreVisit();
			_event.GetObservable()->AcceptVisitor(*this);
			PostVisit();
			_mutex.unlock();
		}
	};
}
