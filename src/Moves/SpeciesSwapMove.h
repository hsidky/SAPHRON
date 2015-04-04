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

		public:
			SpeciesSwapMove() : _prevA(0), _prevB(0), _particlePair{nullptr, nullptr} {} 

			virtual unsigned int RequiredParticles() override { return 2; }

			// Perform move.
			virtual void Perform(const ParticleList& particles) override
			{
				_particlePair.first = particles[0];
				_particlePair.second = particles[1];

				_prevA = particles[0]->GetSpeciesID();
				_prevB = particles[1]->GetSpeciesID();
				particles[0]->SetSpecies(_prevB);
				particles[1]->SetSpecies(_prevA);
			};

			// Undo move.
			virtual void Undo() override
			{
				_particlePair.first->SetSpecies(_prevA);
				_particlePair.second->SetSpecies(_prevB);
			}

			virtual Move* Clone() const override
			{
				return new SpeciesSwapMove(
				               static_cast<const SpeciesSwapMove&>(*this)
				               );
			}
	};
}