#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include "../Observers/Visitable.h"
#include "../JSON/Serializable.h"
#include "ForceField.h"
#include "vecmap.h"
#include <math.h>
#include <map>
#include <iterator>
#include <limits>

namespace SAPHRON
{
	typedef std::pair<int, int> SpeciesPair;
	typedef vecmap<SpeciesPair, ForceField*> FFMap;

	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager : public Visitable, public Serializable
	{
	private:
		//non-bonded forcefields
		FFMap _nonbondedforcefields;

		//bonded forcefields
		FFMap _bondedforcefields;

		//electrostatic forcefield
		FFMap _electrostaticforcefield;

		// Hold unique instances of non-bonded and bonded forcefield pointers.
		FFMap _uniquenbffs;
		FFMap _uniquebffs;
		FFMap _uniqueeffs;

	public:
		typedef FFMap::iterator iterator;
		typedef FFMap::const_iterator const_iterator;
		
		ForceFieldManager() : _nonbondedforcefields(), _bondedforcefields() {}

		// Adds a non-bonded forcefield to the manager.
		void AddNonBondedForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a non-bonded forcefield to the manager.
		void AddNonBondedForceField(int p1type, int p2type, ForceField& ff);

		// Removes a non-bonded forcefield from the manager.
		void RemoveNonBondedForceField(std::string p1type, std::string p2type);

		// Removes a non-bonded forcefield from the manager.
		void RemoveNonBondedForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int NonBondedForceFieldCount();

		// Adds a bonded forcefield to the manager.
		void AddBondedForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a bonded forcefield to the manager.
		void AddBondedForceField(int p1type, int p2type, ForceField& ff);

		// Removes a bonded forcefield from the manager.
		void RemoveBondedForceField(std::string p1type, std::string p2type);

		// Removes a bonded forcefield from the manager.
		void RemoveBondedForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int BondedForceFieldCount();

		// Adds a bonded forcefield to the manager.
		void AddElectrostaticForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a Electrostatic forcefield to the manager.
		void AddElectrostaticForceField(int p1type, int p2type, ForceField& ff);

		// Removes a Electrostatic forcefield from the manager.
		void RemoveElectrostaticForceField(std::string p1type, std::string p2type);

		// Removes a Electrostatic forcefield from the manager.
		void RemoveElectrostaticForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int ElectrostaticForceFieldCount();

		// Evaluates the intermolecular energy of a particle. 
		EPTuple EvaluateInterEnergy(const Particle& particle) const;

		// Evaluates the intermolecular energy of a world.
		EPTuple EvaluateInterEnergy(const World& world) const;

		// Computes the intramolecular energy of a particle, this includes bond
		// energies and connectivities.
		EPTuple EvaluateIntraEnergy(const Particle& particle) const;

		// Computes intramolecular energy of entire world. 
		EPTuple EvaluateIntraEnergy(const World& world) const;

		// Evaluates tail contributions (long range corrections) for a world.
		EPTuple EvaluateTailEnergy(const World& world) const;

		// Evaluates the total energy of a particle including inter
		// and intra.
		EPTuple EvaluateEnergy(const Particle& particle) const;

		// Evaluate total energy of the world including inter, intra
		// and tail.
		EPTuple EvaluateEnergy(const World& world) const;

		// Accept a visitor.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
		}

		// Serialize forcefield manager.
		virtual void Serialize(Json::Value& json) const override;

		// Get (unique) non-bonded forcefields.
		const FFMap& GetNonBondedForceFields() const { return _uniquenbffs;	}

		// Get (unique) bonded forcefields.
		const FFMap& GetBondedForceFields() const { return _uniquebffs;	}
	};
}
