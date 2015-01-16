#pragma once

#include "Lattice3DModel.h"
#include <numeric>

namespace Models
{
	// 3D Lebwohl-Lasher (LL) model. It is a system of uniaxial particles placed
	// at the sites of a cubic lattice that interact through a nearest-neighbor
	// pair potential of H = -J*P2(cos(Ɵ)).
	class LebwohlLasherModel : public Lattice3DModel
	{
		public:

			// Initializes Ising model with a given lattice size (the length of a
			// dimension) and random number seed. The number of stes is the cube
			// of the size. So for a lattice size of 3, there are 9 sites.
			// The sites are initialized on a lattice including
			// positions, nearest neighbors and spins. The default BaseModel
			// parameters are used otherwise.
			using Lattice3DModel::Lattice3DModel;

			using Lattice3DModel::EvaluateHamiltonian;

			double EvaluateHamiltonian(Site& site)
			{
				double h = 0;

				auto& si = site.GetUnitVectors();
				for(int &nindex : site.GetNeighbors())
				{
					auto& sj = Sites[nindex].GetUnitVectors();
					double dot = 0;
					for(size_t i = 0; i < sj.size(); i++)
						dot += si[i]*sj[i];

					// P2 Legendre polynomial
					h += 0.5*(3.0*dot*dot - 1.0);
				}

				return -1 * this->GetInteractionParameter() * h;
			}
	};
}
