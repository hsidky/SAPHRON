#include "Lattice3DModel.h"
#include <cmath>

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
}
