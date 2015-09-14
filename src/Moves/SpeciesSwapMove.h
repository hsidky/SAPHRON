#pragma once 

#include "Rand.h"
#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"

namespace SAPHRON
{
	// Species swap move. This move swaps the species identity between two particles. Note this does not 
	// alter the underlying structure of the particles, but merely exchanges their species identifier.
	class SpeciesSwapMove : public Move
	{
	private:
		Rand _rand;
		int _seed;
		int _rejected;
		int _performed;

	public:
		SpeciesSwapMove(int seed = 2446) :
		_rand(seed), _seed(seed), _rejected(0), _performed(0)  {} 

		// Perform move.
		void Perform(Particle* p1, Particle* p2)
		{
			int i1 = p1->GetSpeciesID();
			int i2 = p2->GetSpeciesID();
			p1->SetSpecies(i2);
			p2->SetSpecies(i1);
		};

		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			// Get two random particles from a random world.
			World* w = wm->GetRandomWorld();
			assert(w->GetParticleCount() > 1);

			Particle* p1 = w->DrawRandomParticle();
			Particle* p2 = w->DrawRandomParticle();

			while(p2 == p1)
				p2 = w->DrawRandomParticle();

			// TODO: FFM is known to double count energies of two particles that are neighbors.
			auto ei = ffm->EvaluateHamiltonian({p1, p2}, w->GetComposition(), w->GetVolume());

			// Increment pulled out since function is called for undo later on.
			Perform(p1, p2);
			++_performed;

			auto ef = ffm->EvaluateHamiltonian({p1, p2}, w->GetComposition(), w->GetVolume());
			Energy de = ef.energy - ei.energy;

			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability. 
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				Perform(p1, p2);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->SetEnergy(w->GetEnergy() + de);
				w->SetPressure(w->GetPressure() + (ef.pressure - ei.pressure));
			}	
		}

		// Perform move using DOS interface.
		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			assert(w->GetParticleCount() > 1);

			Particle* p1 = w->DrawRandomParticle();
			Particle* p2 = w->DrawRandomParticle();

			while(p2 == p1)
				p2 = w->DrawRandomParticle();

			// TODO: FFM is known to double count energies of two particles that are neighbors.
			auto ei = ffm->EvaluateHamiltonian({p1, p2}, w->GetComposition(), w->GetVolume());
			auto opi = op->EvaluateOrderParameter(*w);

			// Increment pulled out since function is called for undo later on.
			Perform(p1, p2);
			++_performed;

			auto ef = ffm->EvaluateHamiltonian({p1, p2}, w->GetComposition(), w->GetVolume());
			auto opf = op->EvaluateOrderParameter(*w);
			Energy de = ef.energy - ei.energy;

			// Acceptance probability. 
			double p = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *w);

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				Perform(p1, p2);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->SetEnergy(w->GetEnergy() + de);
				w->SetPressure(w->GetPressure() + (ef.pressure - ei.pressure));
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

		virtual int GetSeed() const override { return _seed; }

		virtual std::string GetName() const override { return "SpeciesSwap"; }

		virtual Move* Clone() const override
		{
			return new SpeciesSwapMove(
			               static_cast<const SpeciesSwapMove&>(*this)
			               );
		}
	};
}