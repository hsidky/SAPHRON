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
											double) override
		{
			auto& n1 = p1.GetDirectorRef();
			auto& n2 = p2.GetDirectorRef();

			double dot = arma::dot(n1,n2);

			return {-1.0*(_eps*(1.5*dot*dot - 0.5) + _gamma), 0.0};
		}

		// Serialize LJ.
		virtual void Serialize(Json::Value& root) const override
		{
			Json::Value val; 
			val["type"] = "LebwohlLasher";
			val["gamma"] = _gamma;
			val["epsilon"] = _eps;
			root["forcefields"]["nonbonded"].append(val);
		}

		double GetEpsilon() { return _eps; }
		double GetGamma() { return _gamma; }
	};
}
