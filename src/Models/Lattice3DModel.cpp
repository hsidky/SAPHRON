#include "Lattice3DModel.h"
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace Models
{
	// Initializes Lattice3D with the specified lattice dimensions
	// and random number seed. The number of sites is the cube
	// of the size. So for a lattice size of 3x3x3, there are 27 sites.
	// The sites are initialized on a lattice including
	// positions, nearest neighbors and spins. The default BaseModel
	// parameters are used otherwise.
	Lattice3DModel::Lattice3DModel(int xLength,
	                               int yLength,
	                               int zLength,
	                               int seed) :
		BaseModel(xLength * yLength * zLength, seed),
		_xl(xLength), _yl(yLength), _zl(zLength)
	{
		// Initialize positional counters
		double x = 1;
		double y = 1;
		double z = 1;

		for(int i = 0; i < this->GetSiteCount(); i++)
		{
			if(z > _zl)
				z = 1.0;
			if(y > _yl)
				y = 1.0;
			if(x > _xl)
				x = 1.0;

			Sites[i].SetCoordinates(x, y, z);

			// Set nearest neighbors for sites using periodic
			// boundary conditions.
			std::vector<int> neighbors = {
				i - 1, i + 1, // z-axis
				i - _zl, i + _zl, // y-axis
				i - _zl*_yl, i + _zl*_yl // x-axis
			};

			if(z == 1)
				neighbors[0] += _zl;
			else if(z == _zl)
				neighbors[1] -= _zl;
			if(y == 1)
				neighbors[2] += _zl * _yl;
			else if(y == _yl)
				neighbors[3] -= _zl * _yl;
			if(x == 1)
				neighbors[4] += _zl * _yl * _xl;
			else if(x == _xl)
				neighbors[5] -= _zl * _yl * _xl;

			Sites[i].SetNeighbors(neighbors);

			// Initialize all up-spins.
			Sites[i].SetZUnitVector(1.0);

			// Increment counters.
			x += floor((y + z) / (_zl + _yl));
			y += floor(z / _zl);
			z += 1.0;
		}
	}

	// Gets the interaction parameter for species i and j.
	double Lattice3DModel::GetInteractionParameter(int i, int j)
	{
		// Sort max,min.
		if(i < j)
			std::swap(i, j);

		return this->_interactionParameter[i + ((2*_iN - j)*(j-1)/2) -1];
	}

	// Sets the interaction parameter for species i and j. If a value of of
	// i or j is greater than the existing largest interaction parameter "N", then
	// memory is allocated for all values in between "N" and max(i,j). So for example
	// if N = 2 (a11 a12 a22) and i = 3, then the new set of interaction parameters
	// are (a11 a12 a13 a22 a23 a33).
	double Lattice3DModel::SetInteractionParameter(double e, int i, int j)
	{
		int n = (int)_interactionParameter.size();

		// Sort max,min.
		int ni = (i > j) ? i : j;
		int nj = (i < j) ? i : j;

		// Resize vector if needed.
		int nnew = ni*(ni+1)/2;
		if(nnew > n)
		{
			// Store old vars.
			auto oldV = _interactionParameter;
			int oldN = _iN;

			// Resize and update column length.
			_interactionParameter.resize(nnew);
			_iN = ni;

			// Update all of the previous entries
			for(int k = 1; k <= oldN; k++)
				for(int l = 1; l <= oldN; l++)
				{
					int nk = (k > l) ? k : l;
					int nl = (k < l) ? k : l;
					int p = nk + ((2*oldN - nl)*(nl-1)/2) -1;
					SetInteractionParameter(oldV[p], nk, nl);
				}
		}

		// Calculate position.
		int p = ni + ((2*_iN - nj)*(nj-1)/2) -1;

		std::cout << "Interaction parameter set\n"
		          << "Species: " << i << " & " << j << std::fixed << std::setprecision(5)
		          << "  Value: " << e << "\n" << std::endl;

		return this->_interactionParameter[p] = e;
	}
}
