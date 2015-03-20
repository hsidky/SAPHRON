#pragma once

#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Rand.h"
#include "../Worlds/World.h"
#include "NewEnsemble.h"
#include <cmath>

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

			// System energy
			double _energy;

			// Reference to world.
			World& _world;

			// Reference to force field manager.
			ForceFieldManager& _ffmanager;

			// Reference to move manager.
			MoveManager& _mmanager;

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
					auto move = _mmanager.SelectRandomMove();
					move->Perform(*particle);

					// Evaluate energy and accept/reject.
					double currH = _ffmanager.EvaluateHamiltonian(*particle);

					if(AcceptanceProbability(prevH, currH) < _rand.doub())
						move->Undo();
					else
						_energy += (currH - prevH);
				}

				this->IncrementIterations();
				this->NotifyObservers(SimEvent(this, this->GetIteration()));
			}

		public:
			NVTEnsemble(World& world,
			            ForceFieldManager& ffmanager,
			            MoveManager& mmanager,
			            double temperature,
			            int seed = 1) :
				_temperature(temperature), _energy(0.0), _world(world),
				_ffmanager(ffmanager), _mmanager(mmanager), _rand(seed)
			{
				_energy = 0.0;
				// Calculate initial energy.
				for(int i = 0; i < _world.GetParticleCount(); ++i)
				{
					auto particle = _world.SelectParticle(i);
					_energy += _ffmanager.EvaluateHamiltonian(*particle);
				}
				_energy /= 2.0;
			}

			// Run the NVT ensemble fro a specified number of iterations.
			virtual void Run(int iterations) override
			{
				for(int i = 0; i < iterations; ++i)
					Iterate();

				std::cout << "Final energy: " << _energy/_world.GetParticleCount() << std::endl;
			}
	};
}
