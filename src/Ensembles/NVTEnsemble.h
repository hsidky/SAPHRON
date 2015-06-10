#pragma once

#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Rand.h"
#include "../Worlds/World.h"
#include "Ensemble.h"
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

			// System energy and pressure.
			EPTuple _eptuple;

			// Reference to world.
			World& _world;

			// Reference to force field manager.
			ForceFieldManager& _ffmanager;

			// Reference to move manager.
			MoveManager& _mmanager;

			// Random number generator.
			Rand _rand;

			// Particle list.
			ParticleList _particles;

			// Acceptance map.
			AcceptanceMap _accmap;

			inline double AcceptanceProbability(double prevH, double currH)
			{
				double p = exp(-(currH - prevH) / (_temperature*this->GetBoltzmannConstant()));
				return p > 1.0 ? 1.0 : p;
			}

			inline void UpdateAcceptances()
			{
				for(auto& move : _mmanager)
					_accmap[move->GetName()] = move->GetAcceptanceRatio();
			}

			void Iterate();

		protected:
			
			// Visit children.
			virtual void VisitChildren(Visitor& v) override
			{
				_world.AcceptVisitor(v);
			}

		public:
			NVTEnsemble(World& world,
			            ForceFieldManager& ffmanager,
			            MoveManager& mmanager,
			            double temperature,
			            int seed = 1) :
				_temperature(temperature), _eptuple(), _world(world),
				_ffmanager(ffmanager), _mmanager(mmanager), _rand(seed),
				_particles(0), _accmap()
			{

				_particles.reserve(10);
				_eptuple = ffmanager.EvaluateHamiltonian(world);
				UpdateAcceptances();
			}

			// Run the NVT ensemble for a specified number of iterations.
			virtual void Run(int iterations) override;

			// Get temperature.
			virtual double GetTemperature() override 
			{
				return _temperature;
			}

			virtual Pressure GetPressure() override
			{
				Pressure p = _eptuple.pressure;
				p.ideal = (double)_world.GetParticleCount()*GetBoltzmannConstant()*_temperature/_world.GetVolume();
				return p;
			}

			// Get energy.
			virtual Energy GetEnergy() override
			{
				return _eptuple.energy;
			}

			// Get ratio of accepted moves.
			virtual AcceptanceMap GetAcceptanceRatio() override
			{
				return _accmap;
			}
	};
}
