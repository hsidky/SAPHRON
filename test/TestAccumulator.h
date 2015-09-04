#pragma once 
#include "../src/Simulation/SimObserver.h"
#include "../src/Ensembles/Ensemble.h"
#include "../src/Properties/Pressure.h"
#include "../src/Properties/Energy.h"
#include "../src/Worlds/World.h"

namespace SAPHRON
{
	// Class for accumulating averages in a simulation for the purpose of verifying values in unit tests.
	class TestAccumulator : public SimObserver
	{
		private:
			double _temperature; 
			int _counter;
			int _start;
			std::map<World*, double> _density;
			std::map<World*, Energy> _energy;
			std::map<World*, Pressure> _pressure;
			
		public: 
			TestAccumulator(SimFlags flags, unsigned int frequency, unsigned int start) : 
			SimObserver(flags, frequency), _temperature(0),	_counter(0), _start(start),
			_density(), _energy(), _pressure() {}

			virtual void Visit(Ensemble* e) override
			{
				if(e->GetIteration() < _start)
					return;

				++_counter;
				
			}

			virtual void Visit(DOSEnsemble*) override
			{

			}

			virtual void Visit(World* world) override
			{
				if((int)this->GetIteration() < _start)
					return;
				
				if(this->Flags.temperature)
					_temperature += world->GetTemperature();
				
				if(this->Flags.energy)
				{
					if(_energy.find(world) == _energy.end())
						_energy[world] = world->GetEnergy();
					else
						_energy[world] += world->GetEnergy();
				}
				if(this->Flags.pressure)
				{
					if(_pressure.find(world) == _pressure.end())
						_pressure[world] = world->GetPressure();
					else
						_pressure[world] += world->GetPressure();
				}

				if(_density.find(world) == _density.end())
					_density[world]  = world->GetDensity();
				else
					_density[world] += world->GetDensity();
			}

			virtual void Visit(Particle*) override
			{

			}

			virtual void Visit(MoveManager*) override
			{
				
			}

			virtual void Visit(ForceFieldManager*) override
			{
				
			}

			std::map<World*, Energy> GetAverageEnergies()
			{
				auto emap = _energy;
				for(auto& world : emap)
					world.second /= (double)_counter;

				return emap;
			}

			std::map<World*, Pressure> GetAveragePressures()
			{
				auto pmap = _pressure;
				for(auto& world : pmap)
					world.second /= (double)_counter;

				return pmap;
			}
			
			double GetAverageTemperature()
			{
				return _temperature / (double)_counter;
			}

			std::map<World*, double> GetAverageDensities()
			{
				auto dmap = _density;
				for(auto& world : dmap)
					world.second /= (double)_counter;

				return dmap;
			}
	};
}