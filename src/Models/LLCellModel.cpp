#include "LLCellModel.h"
#include <algorithm>
#include <iostream>

namespace Models
{
	// Initializes a LL model in a cell with the specified dimensions.
	// vStart and vEnd represent the unit vectors of the sites at the XY plane
	// where z = 1 and L respectively. The sites at those planes are
	// anchored to those specified vectors for the duration of the simulation.
	LLCellModel::LLCellModel(int xLength, int yLength, int zLength,
	                         std::vector<double> vStart, std::vector<double> vEnd,
	                         int seed) : LebwohlLasherModel(xLength, yLength, zLength, seed)
	{
		// anchor sites at the either face of the xz plane.
		for(unsigned int i = 0; i < Sites.size(); i++)
		{
			auto& site = Sites[i];

			// XY plane where Z = 1.
			if(site.GetZCoordinate() == 1)
			{
				// Add this site to the reserved sites list.
				_reservedSites.push_back(i);
				site.SetUnitVectors(vStart[0], vStart[1], vStart[2]);
			}

			else if(site.GetZCoordinate() == zLength)
			{
				_reservedSites.push_back(i);
				site.SetUnitVectors(vEnd[0], vEnd[1], vEnd[2]);
			}
		}
	}

	// Selects a random site and returns a pointer to the Site object
	// excluding anchored sites.
	Site* LLCellModel::DrawSample()
	{
		// Draw a new sample if we sample the reserved sites. We exploit the fact that
		// since we loop though sites sequentially, the list is sorted.
		int i = this->GetRandomSiteIndex();
		if(std::binary_search(_reservedSites.begin(), _reservedSites.end(), i))
			return DrawSample();

		return &Sites[i];
	}
}
