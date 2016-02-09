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
	using SpeciesPair = std::pair<int, int>;
	using FFMap = vecmap<SpeciesPair, ForceField*>;

	//Forward declare. 
	class Constraint;

	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager : public Visitable, public Serializable
	{
	private:
		// Bon-bonded forcefields.
		FFMap _nonbondedforcefields;

		// Bonded forcefields.
		FFMap _bondedforcefields;

		// Electrostatic forcefield.
		const ForceField* _electroff; 

		// Constraints.
		std::vector<std::vector<Constraint*>> _constraints;

		// Hold unique instances of non-bonded and bonded forcefield pointers.
		FFMap _uniquenbffs;
		FFMap _uniquebffs;

	public:
		typedef FFMap::iterator iterator;
		typedef FFMap::const_iterator const_iterator;
		
		ForceFieldManager() : 
		_nonbondedforcefields(), _bondedforcefields(), _electroff(nullptr), _constraints(0) {}

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

		// Sets the electrostatic forcefield to use in the system.
		void SetElectrostaticForcefield(const ForceField& ff);
		
		// Resets the electrostatic forcefield (to nothing).
		void ResetElectrostaticForceField();

		// Adds a constraint to a species.
		void AddConstraint(std::string species, Constraint* cc);

		// Add a constraint to a species.
		void AddConstraint(int species, Constraint* cc);

		// Resets constraints on a species.
		void ResetConstraints(std::string species);
		
		// Resets constraints on a species.
		void ResetConstraints(int species);

		// Evaluates constraint energy of a particle.
		double EvaluateConstraintEnergy(const Particle& particle) const;

		// Evaluate constraint energy of entire world.
		double EvaluateConstraintEnergy(const World& world) const;

		// Evaluates the intermolecular energy of a particle.
		// This includes constraint energy. 
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
