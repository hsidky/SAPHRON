#pragma once

#include "Move.h"

namespace SAPHRON
{
	// Class for performing a spin flip Monte Carlo move on a particle.
	class FlipSpinMove : public Move
	{
		private:
			Director _prevD;
			Particle* _particle;

		public:

			// Perform the flip spin move on a particle.
			virtual void Perform(Particle& particle) override
			{
				_particle = &particle;
				_prevD = _particle->GetDirector();
				_particle->SetDirector({-1.0*_prevD[0], -1.0*_prevD[1], -1.0*_prevD[2]});
			}

			// Undo the flip spin move on a particle.
			virtual void Undo() override
			{
				_particle->SetDirector(_prevD);
			}

			// Clone move.
			Move* Clone() const override
			{
				return new FlipSpinMove(static_cast<const FlipSpinMove&>(*this));
			}
	};
}
