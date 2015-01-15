#pragma once

#include "../Site.h"
#include "Move.h"
#include <cmath>
#include <random>
#include <vector>

namespace Moves
{
	// Class for selecting a random vector on a sphere for a site.
	// This follows the implementation in Tildesley[1987].
	class SphereUnitVectorMove : public Move<Site>
	{
		private:
			Site* _site;
			std::vector<double> _uBefore;
			std::mt19937_64 generator;
			std::uniform_real_distribution<double> distribution;

		public:
			SphereUnitVectorMove() : distribution(0.0, 1.0){}

			// Selects a new random vector on a sphere.
			void Perform(Site& site)
			{
				_site = &site;
				_uBefore = _site->GetUnitVectors();

				double v3 = 0;
				do
				{
					double v1 = distribution(generator);
					double v2 = distribution(generator);
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
