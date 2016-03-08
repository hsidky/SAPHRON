#pragma once

#include "json/json.h"
#include "../JSON/Serializable.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Worlds/WorldManager.h"

namespace SAPHRON
{
	using ConstraintList = std::vector<Constraint*>;
	// Forward declare.
	class Particle;

	// Abstract base class representing external constraint potential on a world. 
	// Examples of this are harmonic bias (umbrella) or field coupling.
	class Constraint : public Serializable
	{
	protected:
		World* _world;

	public:
		Constraint(World* world) : _world(world) {}

		// Evaluate the energy of the constraint.
		virtual double EvaluateEnergy() const = 0;

		// Serialize the constraint.
		virtual void Serialize(Json::Value& json) const override = 0;

		// Build a constraint potential from a JSON node. Returns a pointer to 
		// the built constraint in addition to adding it to the FFM. If return 
		// value is nullptr, then an unknown error occurred. Will throw BuildException
		// on failure. Object lifetime is caller's responsibility!
		static Constraint* BuildConstraint(
			const Json::Value& json, 
			ForceFieldManager* ffm,
			WorldManager* wm);

		// Overloaded function allowing JSON path specification.
		static Constraint* BuildConstraint(
			const Json::Value& json, 
			ForceFieldManager* ffm, 
			WorldManager* wm, 
			const std::string& path);

		// Builds constraints from base tree json["constraints"] and adds them to the 
		// ForceField manager. It also adds all initialized pointers to the ccl array 
		// passed in. Throws exception on failure. Object lifetimes in vector are 
		// caller's responsibility.
		static void BuildConstraints(
			const Json::Value& json, 
			ForceFieldManager* ffm, 
			WorldManager* wm, 
			ConstraintList& ccl);

		virtual ~Constraint(){}
	};
}
