#pragma once 

#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../NewForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"
#include "../Utils/Rand.h"

namespace SAPHRON
{
	class FlipSpinMove : public Move
	{
	private:
		Rand rand_;
		uint seed_;
	public:
		FlipSpinMove(uint seed = 456326) : 
		rand_(seed), seed_(seed)
		{}

		void Perform(NewParticle* p, uint i)
		{
			auto& di = p->GetDirector(i);
			p->SetDirector(i, -di);
		}

		void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& force) override
		{
			// Get random particle from random world.
			auto* w = wm->GetRandomWorld();
			auto* p = w->DrawRandomParticle();

			// Get random site on particle.
			uint i = rand_.int32() % p->SiteCount();
			auto& s = p->GetSite(i);
			

			// Perform move and evaluate energies.
			auto ei = ffm->EvaluateInterEnergy(s, *w);
			Perform(p, i);
			auto ef = ffm->EvaluateInterEnergy(s, *w);
			this->IncrementAttempts();

			// Acceptance probability. 
			auto& sim = SimInfo::Instance();
			auto de = ef - ei;
			auto P = exp(-de/(w->GetTemperature()*sim.GetkB()));
			P = P > 1.0 ? 1.0 : P;

			// Accept or reject move.
			w->IncrementInterEnergy(de);
			if(force != Accept && (P < rand_.doub() || force == Reject))
			{
				Perform(p, i);
				this->IncrementRejections();
				w->IncrementInterEnergy(-1*de);
			}
		}
	
	};
}