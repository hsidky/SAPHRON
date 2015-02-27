#pragma once

namespace Simulation
{
	class SimObservable;

	class SimFlags
	{
		public:
			SimFlags() : mask(0){}
			union
			{
				struct
				{
					unsigned int temperature : 1;
					unsigned int pressure : 1;
					unsigned int energy : 1;
					unsigned int composition : 1;
					unsigned int iterations : 1;
					unsigned int sweeps : 1;
					unsigned int dos : 1;
					unsigned int histogram : 1;
					unsigned int walkers : 1;
					unsigned int model_interaction_parameter : 1;
					unsigned int model_isotropic_parameter : 1;
					unsigned int site_coordinates : 1;
					unsigned int site_unit_vectors : 1;
					unsigned int site_species : 1;
					unsigned int site_neighbors : 1;
				};
				unsigned int mask;
			};
	};

	class SimEvent
	{
		private:
			SimObservable* _observable;

		public:
			SimEvent(SimObservable* observable) : _observable(observable){}

			// Get Sim Observable.
			SimObservable* GetObservable()
			{
				return _observable;
			}
	};
}
