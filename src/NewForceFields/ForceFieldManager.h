#pragma once

#include "../Utils/Types.h"
#include "NonBondedFF.h"

namespace SAPHRON
{
	// Forward declare. 
	class Particle;
	class NewWorld;

	// Class responsible for managing forcefields and evaluating 
	// energies of particles, worlds, etc..
	class ForceFieldManager 
	{
	private:
		// Non-bonded forcefields.
		std::vector<const NonBondedFF*> nonbondedffs_;
		uint nbcount_;

	public:
		ForceFieldManager() : 
		nonbondedffs_(0), nbcount_(0) {}

		// Adds a non-bonded forcefield to the FF manager. 
		void AddNonBondedForceField(uint s1, uint s2, const NonBondedFF& ff);

		// Removes a non-bonded forcefield from the FF manager.
		void RemoveNonBondedForceField(uint s1, uint s2);

		// Evaluates the intermolecular energy of a site. 
		// The world is needed for traversing neighbors.
		EP EvaluateInterEnergy(const Site& s, const NewWorld& w) const;

		// Evaluates the intermolecular energy of a world.
		EP EvaluateInterEnergy(const NewWorld& w) const;

		// Maps a species pair to an index. 
		inline uint GetIndex(uint i, uint j) const
		{
			return i + j * nbcount_;
		}
	};
}