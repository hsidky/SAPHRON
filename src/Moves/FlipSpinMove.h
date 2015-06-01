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
			int _rejected;
			int _performed;

		public:

			FlipSpinMove() : _rejected(0), _performed(0){} 

			// Perform the flip spin move on a particle.
			virtual void Perform(const ParticleList& particles) override
			{
				_particle = *particles.begin();
				_prevD = _particle->GetDirector();
				_particle->SetDirector(-1.0*_prevD);
				++_performed;
			}

			virtual double GetAcceptanceRatio() override
			{
				return 1.0-(double)_rejected/_performed;
			};

			virtual void ResetAcceptanceRatio() override
			{
				_performed = 0;
				_rejected = 0;
			}

			// Undo the flip spin move on a particle.
			virtual void Undo() override
			{
				_particle->SetDirector(_prevD);
				++_rejected;
			}

			virtual std::string GetName() override
			{
				return "FlipSpin";
			}

			// Clone move.
			Move* Clone() const override
			{
				return new FlipSpinMove(static_cast<const FlipSpinMove&>(*this));
			}
	};
}
