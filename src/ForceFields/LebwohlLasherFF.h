#pragma once

#include "ForceField.h"

namespace SAPHRON
{
	// Class for Lebwohl-Lasher interaction. 
	class LebwohlLasherFF : public ForceField
	{
	private:
		// Interaction parameter.
		double _eps;

		// Isotropic interaction parameter.
		double _gamma;

	public:
		// Inistantiate Lebwohl-Lasher force field.
		LebwohlLasherFF(double eps, double gamma) : _eps(eps), _gamma(gamma) {}

		inline virtual Interaction Evaluate(const Particle& p1, 
											const Particle& p2, 
											const Position&,
											unsigned int) const override
		{
			auto& n1 = p1.GetDirector();
			auto& n2 = p2.GetDirector();

			double dot = fdot(n1,n2);

			return {-1.0*(_eps*(1.5*dot*dot - 0.5) + _gamma), 0.0};
		}

		// Serialize LJ.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "LebwohlLasher";
			json["gamma"] = _gamma;
			json["epsilon"] = _eps;
		}

		double GetEpsilon() { return _eps; }
		double GetGamma() { return _gamma; }
	};
}
