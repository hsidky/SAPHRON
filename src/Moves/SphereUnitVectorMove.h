#pragma once

#include "../Rand.h"
#include "../Site.h"
#include "Move.h"
#include <cmath>
#include <vector>

namespace Moves
{
	// Class for selecting a random unit vector on a sphere for a site.
	class SphereUnitVectorMove : public Move<Site>
	{
		private:
			Site* _site;
			std::vector<double> _uBefore;
			Rand rand;

		public:
			SphereUnitVectorMove(int seed = 23) : _uBefore(3), rand(seed){}

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

				/*
				   double u = 2.0* rand.doub() - 1.0;
				   double v = 2.0* M_PI* rand.doub();
				   _site->SetUnitVectors(sqrt(1.0-u*u)*cos(v), sqrt(1.0-u*u)*sin(v), u);
				 */
			}

			// Undo the move on a site.
			void Undo()
			{
				if(!this->ForceAccept())
					_site->SetUnitVectors(_uBefore[0], _uBefore[1], _uBefore[2]);
			}
	};
}
