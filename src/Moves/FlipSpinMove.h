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
			inline void Perform(Particle* particle)
			{
				_particle = particle;
				_prevD = particle->GetDirector();
				particle->SetDirector(-1.0*_prevD);
				++_performed;
			}
			
			virtual bool Perform(World&, ParticleList& particles) override
			{
				Perform(particles[0]);
				return false;
			}

			virtual void Draw(World& world, ParticleList& particles) override
			{
				particles.resize(1);
				particles[0] = world.DrawRandomParticle();
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
