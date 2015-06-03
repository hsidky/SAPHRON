#pragma once 

#include "Move.h"
#include <utility>

namespace SAPHRON
{
	// Species swap move. This move swaps the species identity between two particles. Note this does not 
	// alter the underlying structure of the particles, but merely exchanges their species identifier.
	class SpeciesSwapMove : public Move
	{
		private:
			int _prevA, _prevB;
			std::pair<Particle*, Particle*> _particlePair;
			int _rejected;
			int _performed;

		public:
			SpeciesSwapMove() : _prevA(0), _prevB(0), _particlePair{nullptr, nullptr} {} 

			// Perform move.
			inline void Perform(Particle* p1, Particle* p2)
			{
				_particlePair.first = p1;
				_particlePair.second = p2;
				_prevA = p1->GetSpeciesID();
				_prevB = p2->GetSpeciesID();
				p1->SetSpecies(_prevB);
				p2->SetSpecies(_prevA);
				++_performed;
			};

			virtual bool Perform(World&, ParticleList& particles) override
			{
				Perform(particles[0], particles[1]);
				return false;
			}

			virtual void Draw(World& world, ParticleList& particles) override
			{
				particles.resize(2);
				particles[0] = world.DrawRandomParticle();
				particles[1] = world.DrawRandomParticle();
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

			// Undo move.
			virtual void Undo() override
			{
				_particlePair.first->SetSpecies(_prevA);
				_particlePair.second->SetSpecies(_prevB);
				++_rejected;
			}
			
			virtual std::string GetName() override { return "SpeciesSwap";	}


			virtual Move* Clone() const override
			{
				return new SpeciesSwapMove(
				               static_cast<const SpeciesSwapMove&>(*this)
				               );
			}
	};
}