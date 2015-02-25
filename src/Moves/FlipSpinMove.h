#pragma once

#include "../Site.h"
#include "Move.h"

namespace Moves
{
	// Class for performing a spin flip Monte Carlo move on a site.
	// Each site will have the Z unit vector changed in sign - as in flipping
	// the spin.
	class FlipSpinMove : public Move<Site>
	{
		private:
			Site* _site;
			double _uzBefore;

		public:

			// Perform the flip spin move on a site.
			void Perform(Site& site)
			{
				_site = &site;
				_uzBefore = _site->GetZUnitVector();
				_site->SetZUnitVector(-1 * _uzBefore);
			}

			// Undo the flip spin move on a site.
			void Undo()
			{
				_site->SetZUnitVector(_uzBefore);
			}

			// Clone move.
			virtual Move* Clone() const
			{
				return new FlipSpinMove(static_cast<const FlipSpinMove&>(*this));
			}
	};
}
