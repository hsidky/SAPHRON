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
			Energy _energy;
			Pressure _pressure;
			int _counter;
			int _start;
			std::map<World*, double> _density;

		public: 
			TestAccumulator(SimFlags flags, unsigned int frequency, unsigned int start) : 
			SimObserver(flags, frequency), _temperature(0), _energy(), _pressure(), 
			_counter(0), _start(start){}

			virtual void Visit(Ensemble* e) override
			{
				if(e->GetIteration() < _start)
					return;

				++_counter;
				if(this->Flags.temperature)
					_temperature += e->GetTemperature();
				if(this->Flags.energy)
					_energy += e->GetEnergy();
				if(this->Flags.pressure)
					_pressure += e->GetPressure();
			}

			virtual void Visit(DOSEnsemble*) override
			{

			}

			virtual void Visit(World* world) override
			{
				if((int)this->GetIteration() < _start)
					return;
				
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


			Energy GetAverageEnergy()
			{
				return _energy / (double)_counter;
			}

			double GetAveragePressure()
			{
				return _pressure.isotropic() / (double)_counter;
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