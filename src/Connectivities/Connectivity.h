#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Particle;

	// Abtract base class representing internal connectivity between groups of particles. For a
	// particle (molecule) that is a group of particles (atoms), this would represent a rigid bond,
	// angle, dihedral, etc...
	class Connectivity
	{
		public:
			// Evaluate the energy of the connectivity.
			virtual double EvaluateEnergy(Particle* p) = 0;

			virtual ~Connectivity(){}
	};
}
