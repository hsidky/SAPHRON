#pragma once

#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../Utils/Rand.h"


namespace SAPHRON
{
	// Class for performing a spin flip Monte Carlo move on a particle.
	class FlipSpinMove : public Move
	{
	private:
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:

		FlipSpinMove(int seed = 76345) 
		: _rand(seed), _rejected(0), _performed(0), _seed(seed) {} 

		void Perform(Particle* p, const Director& d)
		{
			p->SetDirector(-1.0*d);
			++_performed;
		}

		// Internal flip spin move. 
		void Perform(Particle* p)
		{
			auto di = p->GetDirector();
			Perform(p, di);
		}

		// Perform the flip spin move on a particle.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			// Get random particle from random world.
			World* w = wm->GetRandomWorld();
			Particle* particle = w->DrawRandomParticle();
			
			// Get initial director and evaluate energy.
			auto di = particle->GetDirector();
			auto ei = ffm->EvaluateEnergy(*particle);

			// Perform move and evaluate new energy.
			Perform(particle, di);
			auto ef = ffm->EvaluateEnergy(*particle);
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
		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			Particle* particle = w->DrawRandomParticle();
			
			// Get initial director and evaluate energy and order parameter.
			auto di = particle->GetDirector();
			auto ei = ffm->EvaluateEnergy(*particle);
			auto opi = op->EvaluateOrderParameter(*w);

			// Perform move and evaluate new energy and order parameter.
			Perform(particle, di);
			auto ef = ffm->EvaluateEnergy(*particle);
			Energy de = ef.energy - ei.energy;

			// Update energies and pressures.
			w->IncrementEnergy(de);
			w->IncrementPressure(ef.pressure - ei.pressure);

			auto opf = op->EvaluateOrderParameter(*w);

			// Acceptance probability.
			double p = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *w);

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				particle->SetDirector(di);
				
				// Update energies and pressures.
				w->IncrementEnergy(-1.0*de);
				w->IncrementPressure(ei.pressure - ef.pressure);

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

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "FlipSpin";
			json["seed"] = _seed;
		}

		// Get move name.
		virtual std::string GetName() const override { return "FlipSpin"; }

		// Clone move.
		Move* Clone() const override
		{
			return new FlipSpinMove(static_cast<const FlipSpinMove&>(*this));
		}
	};
}
