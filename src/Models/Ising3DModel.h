#pragma once

#include "Lattice3DModel.h"

namespace Models
{
	// 3D Ising model. It is an n-by-n-by-n lattice where each site can have an
	// up-spin or down-spin. The z unit vector in BaseModel was arbitrarily chosen
	// to hold the spin data. The default exchange energy is 1 (ferromagnetic).
	class Ising3DModel : public Lattice3DModel
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
			// Evaluates the Ising Hamiltonian for a given site using the formula
			// H = -J*sum(si*sj). Since outside the "sphere of influence" the interaction
			// energy doesn't change (i.e. only two body interactions), we only need to
			// evaluate nearest neighbors.
			double EvaluateHamiltonian(Site& site)
			{
				double h = 0;
				auto si = site.GetZUnitVector();
				for(int &nindex : site.GetNeighbors())
					// Force only -1 or 1 to prevent drift.
					h += si * Sites[nindex].GetZUnitVector() < 0 ? -1.0 : 1.0;

				h *= -1 * this->GetInteractionParameter();
				return h;
			}
	};
}
