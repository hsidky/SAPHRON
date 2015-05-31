#pragma once

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Identity change move. This move merely changes the identity of a particle. Meaning that it is useful
	// when performing (semi)grand canonical simulations on spins or atoms, but NOT on moleucles or
	// composite particle types. The only thing altered is the species and string.
	class IdentityChangeMove : public Move
	{
		private:
			Particle* _particle;

			int _prevSpecies;

			Rand _rand;

			int _speciesCount;
			int _rejected;
			int _performed;

		public:
			IdentityChangeMove(int speciesCount, int seed = 1337) :
				_particle(nullptr), _prevSpecies(0), _rand(seed), _speciesCount(speciesCount),
				_rejected(0), _performed(0) {}

			// Reassigns the species of a site with a new random one.
			virtual void Perform(const ParticleList& particles) override
			{
				_particle = *particles.begin();

				// Record previous species
				_prevSpecies = _particle->GetSpeciesID();

				_particle->SetSpecies(_rand.int32() % _speciesCount);
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

			// Undo the move on a site.
			void Undo() override
			{
				_particle->SetSpecies(_prevSpecies);
				++_rejected;
			}

			virtual std::string GetName() override { return "IdentityChange";	}

			// Clone move.
			virtual Move* Clone() const override
			{
				return new IdentityChangeMove(
				               static_cast<const IdentityChangeMove&>(*this)
				               );
			}
	};
}
