#pragma once

#include "../Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../Simulation/SimInfo.h"
#include "Move.h"
#include <cmath>

namespace SAPHRON
{

	// Class performs a random rotation of a particle director.
	// This is effectively equivalent to picking a random vector on a 
	// unit sphere.
	class DirectorRotateMove : public Move
	{
	private:
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:
		DirectorRotateMove (int seed = 3) 
		: _rand(seed), _rejected(0), _performed(0), _seed(seed) {}

		// Director rotation. Used for unit testing. 
		void Perform(Particle* particle)
		{
			// Get new unit vector.
			double v3 = 0;
			do
			{
				double v1 = _rand.doub();
				double v2 = _rand.doub();
				v1 = 1 - 2 * v1;
				v2 = 1 - 2 * v2;
				v3 = v1*v1 + v2*v2;
				if(v3 < 1)
					particle->SetDirector(2.0*v1*sqrt(1 - v3), 2.0*v2*sqrt(1 - v3), 1.0-2.0*v3);
			} while(v3 > 1);

			++_performed;
		}

		// Perform a random director rotation.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			// Select random particle from random world. 
			World* w = wm->GetRandomWorld();
			Particle* particle = w->DrawRandomParticle();

			// Get initial director, energy.
			Director di = particle->GetDirector();
			auto ei = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());

			// Perform director rotation.
			Perform(particle);

			// Evaluate final energy and check probability.
			auto ef = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());
			Energy de = ef.energy - ei.energy;
				
			// Get sim info for kB.
			auto sim = SimInfo::Instance();

			// Acceptance probability.
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				particle->SetDirector(di);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de);
				w->IncrementPressure(ef.pressure - ei.pressure);
			}	

		}

		// Perform move using DOS interface. 
		virtual void Perform(World* world, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			Particle* particle = world->DrawRandomParticle();

			Director di = particle->GetDirector();
			auto ei = ffm->EvaluateHamiltonian(*particle, world->GetComposition(), world->GetVolume());
			auto opi = op->EvaluateOrderParameter(*world);
			
			// Perform
			Perform(particle);

			// Evaluate final energy and order parameter and check probability. 
			auto ef = ffm->EvaluateHamiltonian(*particle, world->GetComposition(), world->GetVolume());
			Energy de = ef.energy - ei.energy;

			// Update energies and pressures.
			world->IncrementEnergy(de);
			world->IncrementPressure(ef.pressure - ei.pressure);

			auto opf = op->EvaluateOrderParameter(*world);

			// Evaluate acceptance probability. 
			double p = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *world);

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				// Update energies and pressures.
				world->IncrementEnergy(-1.0*de);
				world->IncrementPressure(ei.pressure - ef.pressure);

				particle->SetDirector(di);
				++_rejected;
			}
		}

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Get seed.
		virtual int GetSeed() const override { return _seed; }

		virtual std::string GetName() const override { return "DirectorRotate";	}

		// Clone move.
		Move* Clone() const override
		{
			return new DirectorRotateMove(static_cast<const DirectorRotateMove&>(*this));
		}
	};
}
