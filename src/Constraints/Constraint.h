#pragma once

#include "json/json.h"
#include "../JSON/Serializable.h"

namespace SAPHRON
{
	// Forward declare.
	class Particle;

	// Abtract base class representing external constraint potential on a particle. 
	// Examples of this are harmonic bias (umbrella) or field coupling.
	class Constraint : public Serializable
	{
		public:
			// Evaluate the energy of the constraint.
			virtual double EvaluateEnergy(const Particle& p) const = 0;

			virtual void Serialize(Json::Value& json) const override = 0;

			virtual ~Constraint(){}
	};
}
