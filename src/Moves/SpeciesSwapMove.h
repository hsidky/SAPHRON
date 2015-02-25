#pragma once

#include "../Rand.h"
#include "../Site.h"
#include "Move.h"

namespace Moves
{
	// Class for random swapping the species type of a site.
	class SpeciesSwapMove : public Move<Site>
	{
		private:
			Site* _site;
			int _prevSpecies;
			Rand rand;
			int _speciesCount;

		public:
			SpeciesSwapMove(int speciesCount, int seed = 1337) :
				rand(seed), _speciesCount(speciesCount) {}

			// Reassigns the species of a site with a new random one.
			void Perform(Site& site)
			{
				_site = &site;

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
