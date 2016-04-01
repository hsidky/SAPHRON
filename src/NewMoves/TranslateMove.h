#pragma once 

#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../NewForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"
#include "../Utils/Rand.h"

namespace SAPHRON
{
	// Class for translating a random particle a maximum of "dx" distance.
	class TranslateMove : public Move
	{
	private:
		double dx_;
		Rand rand_;
		uint seed_;
	
	public:
		TranslateMove(double dx, uint seed = 398374) : 
		dx_(dx), rand_(seed), seed_(seed)
		{}

		void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& force) override
		{
			// Get random particle from random world.
			auto* w = wm->GetRandomWorld();
			auto* p = w->DrawRandomParticle();
			auto posi = p->GetPosition();

			// Perform move and evaluate energies.
			auto ei = ffm->EvaluateInterEnergy(*p, *w);
			
			Vector3 posn = {
				posi[0] + dx_*(rand_.doub()-0.5),
				posi[1] + dx_*(rand_.doub()-0.5),
				posi[2] + dx_*(rand_.doub()-0.5)
			};
			w->ApplyPeriodicBoundaries(posn);
			p->SetPosition(posn);

			auto ef = ffm->EvaluateInterEnergy(*p, *w);
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
				p->SetPosition(posi);
				this->IncrementRejections();
				w->IncrementInterEnergy(-1*de);
			}
			else
				w->CheckCellListUpdate(*p);
		}
	
	};
}