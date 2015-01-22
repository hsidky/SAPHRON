#pragma once

#include "LebwohlLasherModel.h"

namespace Models
{
	// 3D Lebwohl-Lasher (LL) model confined in a cell with anchoring at the walls. It is a system of uniaxial
	// particles placed at the sites of a cubic lattice that interact through a nearest-neighbor
	// pair potential of H = -J*P2(cos(Ɵ)).
	class LLCellModel : public LebwohlLasherModel
	{
		private:
			// Sites that are anchored and thus not drawn in a sampling.
			std::vector<int> _reservedSites;

		public:

			// Initializes a LL model in a cell with the specified dimensions.
			// vStart and vEnd represent the unit vectors of the sites at the YZ plane
			// where x = 1 and L respectively. The sites at those planes are
			// anchored to those specified vectors for the duration of the simulation.
			LLCellModel(int xLength,
			            int yLength,
			            int zLength,
			            std::vector<double> vStart,
			            std::vector<double> vEnd,
			            int seed = 1);

			// Selects a random site and returns a pointer to the Site object
			// excluding anchored sites.
			virtual Site* DrawSample();

            // Get indices of all reserved sites - that is, sites that are never
            // drawn during sampling.
			std::vector<int> GetReservedSites()
			{
				return _reservedSites;
			}
	};
}
