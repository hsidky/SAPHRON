#pragma once

namespace SAPHRON
{
	class SimObservable;

	class SimFlags
	{
		public:
			SimFlags() : simulation(0), world(0), dos(0), histogram(0), particle(0) {}
			union
			{
				struct
				{
					unsigned int iteration : 1;
					unsigned int move_acceptances: 1;
				};

				unsigned int simulation;
			};

			union
			{
				struct
				{
					unsigned int world_temperature: 1;
					unsigned int world_pressure: 1;
					unsigned int world_volume : 1;
					unsigned int world_density : 1;
					unsigned int world_energy : 1;
					unsigned int world_composition: 1;
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
					unsigned int particle_id : 1;
					unsigned int particle_species : 1;
					unsigned int particle_parent_id : 1;
					unsigned int particle_parent_species : 1;
					unsigned int particle_position : 1;
					unsigned int particle_director : 1;
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
			SimObservable const * GetObservable() const { return _observable; }

			// Get iteration.
			int GetIteration() const { return _iteration; }

			// Tell the observer that they should observe this event.
			bool ForceObserve() const { return _forceObserve; }
	};
}
