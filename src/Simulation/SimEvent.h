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
					unsigned int dos_histogram : 1;
					unsigned int dos_bin_count : 1;
					unsigned int dos_walker : 1;
					unsigned int dos_flatness : 1;
					unsigned int dos_scale_factor : 1;
					unsigned int dos_interval : 1;
					unsigned int dos_lower_outliers : 1;
					unsigned int dos_upper_outliers : 1;
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
			bool _forceObserve = false;

		public:
			SimEvent(SimObservable* observable, bool forceObserve = false) 
				: _observable(observable), _forceObserve(forceObserve){}

			// Get Sim Observable.
			SimObservable* GetObservable()
			{
				return _observable;
			}

			// Tell the observer that they should observe this event.
			bool ForceObserve()
			{
				return _forceObserve;
			}
	};
}
