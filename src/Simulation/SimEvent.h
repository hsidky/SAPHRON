#pragma once

namespace Simulation
{
	class SimObservable;

	class SimEvent
	{
		private:
			SimObservable* _observable;

		public:
			SimEvent(SimObservable* observable) : _observable(observable){}
	};
}
