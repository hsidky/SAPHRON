#pragma once 
#include "../src/Simulation/SimObserver.h"
#include "../src/Simulation/Simulation.h"
#include "../src/Properties/Pressure.h"
#include "../src/Properties/Energy.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"

namespace SAPHRON
{
	// Class for accumulating averages in a simulation for the purpose of verifying values in unit tests.
	class TestAccumulator : public SimObserver
	{
		private:
			double _temperature; 
			unsigned _counter;
			unsigned _start;
			std::map<World*, double> _density;
			std::map<World*, Energy> _energy;
			std::map<World*, Pressure> _pressure;
			std::map<World*, double> _chemicalpotential;
			
		public: 
			TestAccumulator(SimFlags flags, unsigned int frequency, unsigned int start) : 
			SimObserver(flags, frequency), _temperature(0),	_counter(0), _start(start),
			_density(), _energy(), _pressure() {}

			virtual std::string GetName() const override { return "TestAccumulator"; }

			virtual void Visit(const Simulation& e) override
			{
				if(e.GetIteration() < _start)
					return;

				++_counter;
				
			}

			virtual void Visit(const DOSSimulation&) override
			{

			}

			virtual void Visit(const WorldManager& wm) override
			{
				if(this->GetIteration() < _start)
					return;
				
				for(auto& world : wm)
				{
					if(this->Flags.world_temperature)
						_temperature += world->GetTemperature();
				
					if(this->Flags.world_energy)
					{
						if(_energy.find(world) == _energy.end())
							_energy[world] = world->GetEnergy();
						else
							_energy[world] += world->GetEnergy();
					}
					if(this->Flags.world_pressure)
					{
						if(_pressure.find(world) == _pressure.end())
							_pressure[world] = world->GetPressure();
						else
							_pressure[world] += world->GetPressure();
					}
					if(this->Flags.world_chem_pot)
					{
						if(_chemicalpotential.find(world) == _chemicalpotential.end())
							_chemicalpotential[world] = world->GetChemicalPotential(0);
						else
							_chemicalpotential[world] += world->GetChemicalPotential(0);
					}

					if(_density.find(world) == _density.end())
						_density[world]  = world->GetNumberDensity();
					else
						_density[world] += world->GetNumberDensity();
				}

				
			}

			virtual void Visit(const Particle&) override
			{

			}

			virtual void Visit(const MoveManager&) override
			{
				
			}

			virtual void Visit(const Histogram&) override
			{
				
			}

			virtual void Visit(const ForceFieldManager&) override
			{
				
			}

			virtual void Serialize(Json::Value&) const override
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

			std::map<World*, double> GetAverageChemicalPotential()
			{
				auto mumap = _chemicalpotential;
				for(auto& world : mumap)
					world.second /= (double)_counter;

				return mumap;
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