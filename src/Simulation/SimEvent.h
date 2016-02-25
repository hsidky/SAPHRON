#pragma once

#include "../JSON/Serializable.h"

namespace SAPHRON
{
	class SimObservable;

	class SimFlags : public Serializable
	{
		private:
			unsigned int simulation_mask = 31;
			unsigned int world_mask = 127;
			unsigned int energy_mask = 63;
			unsigned int pressure_mask = 255;
			unsigned int histogram_mask = 63;
			unsigned int particle_mask = 255;

		public:
			SimFlags() : simulation(0), world(0), energy_components(0), 
			pressure_tensor(0), histogram(0), particle(0) {}

			void simulation_on() { simulation = simulation_mask; }
			void world_on() { world = world_mask; }
			void energy_on() {energy_components = energy_mask; }
			void pressure_on() { pressure_tensor = pressure_mask; }
			void histogram_on() { histogram = histogram_mask; }
			void particle_on() { particle = particle_mask; }

			union
			{
				struct
				{
					unsigned int iteration : 1;
					unsigned int move_acceptances: 1;
					unsigned int dos_factor : 1;
					unsigned int dos_flatness : 1;
					unsigned int dos_op : 1;
				};

				unsigned int simulation;
			};

			union
			{
				struct
				{

					unsigned int world_pressure: 1;
					unsigned int world_volume : 1;
					unsigned int world_density : 1;
					unsigned int world_temperature: 1;
					unsigned int world_composition: 1;
					unsigned int world_energy : 1;
					unsigned int world_chem_pot : 1;
				};
				
				unsigned int world;
			};

			union
			{
				struct
				{
					unsigned int eintervdw: 1;
					unsigned int eintravdw: 1;
					unsigned int einterelect: 1;
					unsigned int eintraelect: 1;
					unsigned int ebonded: 1;
					unsigned int econnectivity: 1;
				};
				unsigned int energy_components;
			};

			union
			{
				struct
				{
					unsigned int pideal : 1;
					unsigned int pxx: 1;
					unsigned int pxy: 1;
					unsigned int pxz: 1;
					unsigned int pyy: 1;
					unsigned int pyz: 1;
					unsigned int pzz: 1;
					unsigned int ptail: 1;
				};
			
				unsigned int pressure_tensor;
			};

			union
			{
				struct
				{
					unsigned int hist_interval : 1;
					unsigned int hist_bin_count : 1;
					unsigned int hist_lower_outliers : 1;
					unsigned int hist_upper_outliers : 1;
					unsigned int hist_values : 1;
					unsigned int hist_counts : 1;
				};
				unsigned int histogram;
			};

			union
			{
				struct
				{
					unsigned int particle_id : 1;
					unsigned int particle_species : 1;
					unsigned int particle_species_id : 1;
					unsigned int particle_parent_id : 1;
					unsigned int particle_parent_species : 1;
					unsigned int particle_charge : 1;
					unsigned int particle_position : 1;
					unsigned int particle_director : 1;
				};
				unsigned int particle;
			};

			virtual void Serialize(Json::Value& json) const override
			{
				if(simulation == simulation_mask)
					json["simulation"] = 1;
				else
				{
					if(iteration) json["iteration"] = 1;
					if(move_acceptances) json["move_acceptances"] = 1;
					if(dos_factor) json["dos_factor"] = 1;
					if(dos_flatness) json["dos_flatness"] = 1;
					if(dos_op) json["dos_op"] = 1;
				}

				if(world == world_mask)
					json["world"] = 1;
				else
				{
					if(world_pressure) json["world_pressure"] = 1;
					if(world_volume) json["world_volume"] = 1;
					if(world_density) json["world_density"] = 1;
					if(world_temperature) json["world_temperature"] = 1;
					if(world_composition) json["world_composition"] = 1;
					if(world_energy) json["world_energy"] = 1;
					if(world_chem_pot) json["world_chem_pot"] = 1;
				}

				if(energy_components == energy_mask)
					json["energy_components"] = 1;
				else
				{
					if(eintervdw) json["energy_intervdw"] = 1;
					if(eintravdw) json["energy_intravdw"] = 1;
					if(einterelect) json["energy_intraelect"] = 1;
					if(ebonded) json["energy_bonded"] = 1;
					if(econnectivity) json["energy_connectivity"] = 1;
				}

				if(pressure_tensor == pressure_mask)
					json["pressure_tensor"] = 1;
				else
				{
					if(pideal) json["pressure_ideal"] = 1;
					if(pxx) json["pressure_pxx"] = 1;
					if(pxy) json["pressure_pxy"] = 1;
					if(pxz) json["pressure_pxz"] = 1;
					if(pyy) json["pressure_pyy"] = 1;
					if(pyz) json["pressure_pyz"] = 1;
					if(ptail) json["pressure_tail"] = 1;
				}

				if(histogram == histogram_mask)
					json["histogram"] = 1;
				else
				{
					if(hist_interval) json["hist_interval"] = 1;
					if(hist_bin_count) json["hist_bin_count"] = 1;
					if(hist_lower_outliers) json["hist_lower_outliers"] = 1;
					if(hist_upper_outliers) json["hist_upper_outliers"] = 1;
					if(hist_values) json["hist_values"] = 1;
					if(hist_counts) json["hist_counts"] = 1;
				}

				if(particle == particle_mask)
					json["particle"] = 1;
				else
				{
					if(particle_id) json["particle_id"] = 1;
					if(particle_species) json["particle_species"] = 1;
					if(particle_species_id) json["particle_species_id"] = 1;
					if(particle_parent_id) json["particle_parent_id"] = 1;
					if(particle_parent_species) json["particle_parent_species"] = 1;
					if(particle_charge) json["particle_charge"] = 1;
					if(particle_position) json["particle_position"] = 1;
					if(particle_director) json["particle_director"] = 1;
				}
			}
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
