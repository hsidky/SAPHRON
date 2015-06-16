#pragma once

namespace SAPHRON
{
	class SimObservable;

	class SimFlags
	{
		public:
			SimFlags() : ensemble(0), world(0), dos(0), histogram(0), particle(0) {}
			union
			{
				struct
				{
					unsigned int identifier : 1;
					unsigned int iterations : 1;
					unsigned int energy : 1;
					unsigned int temperature : 1;
					unsigned int pressure : 1;
					unsigned int composition : 1;
					unsigned int acceptance: 1;
				};

				unsigned int ensemble;
			};

			union
			{
				struct
				{
					unsigned int world_volume : 1;
					unsigned int world_density : 1;
					unsigned int world_count : 1;
				};

				unsigned int world;
			};

			union
			{
				struct
				{
					unsigned int dos_walker : 1;
					unsigned int dos_scale_factor : 1;
					unsigned int dos_flatness : 1;
					unsigned int dos_interval : 1;
					unsigned int dos_bin_count: 1;
					unsigned int dos_values : 1;
				};
				unsigned int dos;
			};

			union
			{
				struct
				{
					unsigned int hist_bin_count : 1;
					unsigned int hist_lower_outliers : 1;
					unsigned int hist_upper_outliers : 1;
					unsigned int hist_values : 1;
				};
				unsigned int histogram;
			};

			union
			{
				struct
				{
					unsigned int particle_global_id : 1;
					unsigned int particle_position : 1;
					unsigned int particle_director : 1;
					unsigned int particle_species : 1;
					unsigned int particle_species_id : 1;
					unsigned int particle_neighbors : 1;
				};
				bool particle;
			};
	};

	class SimEvent
	{
		private:
			SimObservable* _observable;
			int _iteration;
			bool _forceObserve = false;

		public:
			SimEvent(SimObservable* observable, int iteration, bool forceObserve = false)
				: _observable(observable), _iteration(iteration), _forceObserve(forceObserve){}

			// Get Sim Observable.
			SimObservable* GetObservable()
			{
				return _observable;
			}

			// Get iteration.
			int GetIteration()
			{
				return _iteration;
			}

			// Tell the observer that they should observe this event.
			bool ForceObserve()
			{
				return _forceObserve;
			}
	};
}
