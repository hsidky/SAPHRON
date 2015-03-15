#pragma once 

#include "NewEnsemble.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Class for simple NVT ensemble - also known as "canonical" ensemble.This
	// calls moves off the move queue and uses the Metropolis criteria for acceptance
	// probability, A(i->j) = min(1,exp(DE/kB*T)).
	class NVTEnsemble : public Ensemble
	{
	private:

		// System temperature.
		double _temperature;

		// Reference to world.
		World& _world;

		// Reference to force field manager.
		ForceFieldManager& _ffmanager;

		// Random number generator.
		Rand _rand;
		
		inline double AcceptanceProbability(double prevH, double currH)
		{
			double p = exp(-(currH - prevH) / (_temperature*this->GetBoltzmannConstant()));
			return p > 1.0 ? 1.0 : p;
		}

		void Iterate()
		{
			for (int i = 0; i < _world.GetParticleCount(); ++i)
			{
				// Draw sample, evaluate energy.
				auto particle = _world.DrawRandomParticle();
				double prevH = _ffmanager.EvaluateHamiltonian(*particle);

				// Select a random move and perform. 
				_rand
			}
		}

	public:
		NVTEnsemble(World& world, ForceFieldManager& ffmanager, double temperature, int seed = 1) :
			_world(world), _ffmanager(ffmanager), _temperature(temperature), _rand(seed){}
		
		// Run the NVT ensemble fro a specified number of iterations.
		virtual void Run(int iterations) override
		{
			
		}
	};
}