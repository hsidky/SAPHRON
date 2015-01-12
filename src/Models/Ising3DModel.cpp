#include "Ising3DModel.h"
#include <cmath>

namespace Models
{
	// Initializes Ising3D with a given lattice size (the length of a
	// dimension) and random number seed. The number of sites is the
	// cube of the size. So for a lattice size of 3, there are 9 sites.
	// The sites are initialized on a lattice including positions,
	// nearest neighbors and spins. The default BaseModel parameters
	// are used.
	Ising3DModel::Ising3DModel(int latticeSize, double temperature, int seed) :
		BaseModel(latticeSize * latticeSize * latticeSize, temperature,
		          seed), _latticeSize(latticeSize)
	{
		// Length of lattice side
		auto n = this->_latticeSize;

		// Initialize positional counters
		double x = 1;
		double y = 1;
		double z = 1;

		for(int i = 0; i < this->GetSiteCount(); i++)
		{
			if(z > n)
				z = 1.0;
			if(y > n)
				y = 1.0;
			if(x > n)
				x = 1.0;

			Sites[i].SetCoordinates(x, y, z);

			// Set nearest neighbors for sites using periodic
			// boundary conditions.
			std::vector<int> neighbors = {
				i - 1, i + 1, // z-axis
				i - n, i + n, // y-axis
				i - n * n, i + n * n // x-axis
			};

			if(z == 1)
				neighbors[0] += n;
			else if(z == n)
				neighbors[1] -= n;
			if(y == 1)
				neighbors[2] += n * n;
			else if(y == n)
				neighbors[3] -= n * n;
			if(x == 1)
				neighbors[4] += n * n * n;
			else if(x == n)
				neighbors[5] -= n * n * n;

			Sites[i].SetNeighbors(neighbors);

			// Initialize all up-spins.
			Sites[i].SetZUnitVector(1.0);

			// Increment counters.
			x += floor((y + z) / (2.0 * n));
			y += floor(z / n);
			z += 1.0;
		}
	}

	// Evaluates the Ising Hamiltonian for a given site using the formula
	// H = -J*sum(si*sj) where si is the spin for the site at index. Since outside
	// the "sphere of influence" the interaction energy doesn't change (i.e. only
	// two body interactions), we only need to evaluate nearest neighbors.
	double Ising3DModel::EvaluateHamiltonian(int index)
	{
		return this->EvaluateHamiltonian(&Sites[index]);
	}

	// Evaluates the Ising Hamiltonian for a given site using the formula
	// H = -J*sum(si*sj). Since outside the "sphere of influence" the interaction
	// energy doesn't change (i.e. only two body interactions), we only need to
	// evaluate nearest neighbors.
	double Ising3DModel::EvaluateHamiltonian(Site* site)
	{
		double h = 0;
		auto si = site->GetZUnitVector();
		for(int &nindex : site->GetNeighbors())
			// Force only -1 or 1 to prevent drift.
			h += si * Sites[nindex].GetZUnitVector() < 0 ? -1.0 : 1.0;

		h *= -1 * this->GetInteractionParameter();
		return h;
	}
}
