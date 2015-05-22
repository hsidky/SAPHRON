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
			// Evaluate the Hamiltonian of the connectivity.
			virtual double EvaluateHamiltonian(Particle* p) = 0;

			virtual ~Connectivity(){}
	};
}
