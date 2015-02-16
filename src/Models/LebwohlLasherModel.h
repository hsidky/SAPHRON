#pragma once

#include "Lattice3DModel.h"
#include <iostream>
#include <numeric>

namespace Models
{
	// 3D Lebwohl-Lasher (LL) model. It is a system of uniaxial particles placed
	// at the sites of a cubic lattice that interact through a nearest-neighbor
	// pair potential of H = Ʃ(γij + εij*P2(cosθij)).
	class LebwohlLasherModel : public Lattice3DModel
	{
		private:
			// Isotropic mixing parameter γij.
			std::vector<double> _isotropicJ = {0.0};

			// Column size of the (ghost) isotropic interaction parameter matrix.
			int _isoN = 1;

		public:

			// Initializes LL model with a given lattice size (the length of a
			// dimension) and random number seed. The number of stes is the cube
			// of the size. So for a lattice size of 3, there are 9 sites.
			// The sites are initialized on a lattice including
			// positions, nearest neighbors and spins. The default BaseModel
			// parameters are used otherwise.
			using Lattice3DModel::Lattice3DModel;

			using Lattice3DModel::EvaluateHamiltonian;

			// Configure a Lebwohl-Lasher mixture with a certian number of species and
			// targtet mole fractions. The species will be randomly distributed throughout
			// the lattice and the specified mole fractions will only be met approximately
			// since it may not be possible to get a perfect match.
			void ConfigureMixture(int speciesCount, std::vector<double> moleFractions);

			// Evaluate the LL Hamiltonian H = Ʃ(γij + εij*P2(cosθij)).
			double EvaluateHamiltonian(Site& site);

			// Gets the isotropic interaction parameter, γij, between two species.
			double GetIsotropicParameter(int i = 1, int j = 1);

			// Sets the isotropic interaction parameter, γij, between two species.
			// If a value of of i or j is greater than the existing largest interaction
			// parameter "N", then memory is allocated for all values in between "N" and
			// max(i,j). So for example if N = 2 (a11 a12 a22) and i = 3, then the new
			// set of interaction parameters are (a11 a12 a13 a22 a23 a33).
			double SetIsotropicParameter(double e, int i, int j);
	};
}
