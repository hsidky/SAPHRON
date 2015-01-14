#pragma once

#include "BaseModel.h"

namespace Models
{
	// 3D lattice model. It is an abstract class that can be overriden with a particular
	// Hamiltonian for the appropriate model.
	class Lattice3DModel : public BaseModel
	{
		private:

			// Size of since lattice dimension.
			int _latticeSize;

			// Normalized "J", interaction parameter (J) such that a specified
			// temperature is reduced with a normalized kb from BaseModel.
			double _interactionParameter = 1.0;
		public:

			// Initializes Lattice3D with a given lattice size (the length of a
			// dimension) and random number seed. The number of stes is the cube
			// of the size. So for a lattice size of 3, there are 9 sites.
			// The sites are initialized on a lattice including
			// positions, nearest neighbors and spins. The default BaseModel
			// parameters are used otherwise.
			Lattice3DModel(int latticeSize, int seed = 1);

			// Sets the interaction parameter in the Hamiltonian.
			double SetInteractionParameter(double j)
			{
				return this->_interactionParameter = j;
			};

			// Gets the interaction parameter in the Hamiltonian.
			double GetInteractionParameter()
			{
				return this->_interactionParameter;
			};

			// Gets the lattice size (length of a single dimension).
			int GetLatticeSize()
			{
				return this->_latticeSize;
			}
	};
}
