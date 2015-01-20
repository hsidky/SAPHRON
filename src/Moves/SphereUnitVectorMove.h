#pragma once

#include "../Rand.h"
#include "../Site.h"
#include "Move.h"
#include <cmath>
#include <random>
#include <vector>

namespace Moves
{
	// Class for selecting a random unit vector on a sphere for a site.
	// This follows the implementation in Tildesley[1987].
	class SphereUnitVectorMove : public Move<Site>
	{
		private:
			Site* _site;
			std::vector<double> _uBefore;
			Rand rand;

		public:
			SphereUnitVectorMove() : _uBefore(3), rand(3){}

			// Selects a new random unit vector on a sphere.
			void Perform(Site& site)
			{
				_site = &site;

				// Record previous locations.
				auto& uv = _site->GetUnitVectors();
				for(int i = 0; i < 3; i++)
					_uBefore[i] = uv[i];

				// Get new unit vector.
				double v3 = 0;
				do
				{
					double v1 = rand.doub();
					double v2 = rand.doub();
					v1 = 1 - 2 * v1;
					v2 = 1 - 2 * v2;
					v3 = v1*v1 + v2*v2;
					if(v3 < 1)
						_site->SetUnitVectors(
						        2*v1*sqrt(1 - v3), 2*v2*sqrt(1 - v3),1-2*v3);
				} while(v3 > 1);
			}

			// Undo the move on a site.
			void Undo()
			{
				_site->SetUnitVectors(_uBefore[0], _uBefore[1], _uBefore[2]);
			}
	};
}
