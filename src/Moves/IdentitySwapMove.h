#pragma once

#include "../Rand.h"
#include "NewMove.h"

namespace SAPHRON
{
	// Identity swap move. This move merely swaps the identity of a particle. Meaning that it is useful
	// when performing (semi)grand canonical simulations on spins or atoms, but NOT on moleucles or
	// composite particle types. The only thing altered is the identifier and string.
	class IdentitySwapMove : public Move
	{
		private:
			Particle* _particle;

			std::string _prevSpecies;

			Rand _rand;
			int _speciesCount;

		public:
			SpeciesSwapMove(int speciesCount, int seed = 1337) :
				rand(seed), _speciesCount(speciesCount) {}

			// Reassigns the species of a site with a new random one.
			virtual void Perform(Particle& particle) override
			{
				_particle = &particle;

				// Record previous species
				_prevSpecies = _site->GetSpecies();

				// Select a random species
				int newSpecies =  rand.int32() % _speciesCount + 1;

				_site->SetSpecies(newSpecies);
			}

			// Undo the move on a site.
			void Undo()
			{
				_site->SetSpecies(_prevSpecies);
			}

			// Clone move.
			virtual Move* Clone() const
			{
				return new SpeciesSwapMove(
				               static_cast<const SpeciesSwapMove&>(*this)
				               );
			}
	};
}
