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

			inline virtual Interaction Evaluate(const Particle& p1, const Particle& p2, const Position&) override
			{
				auto& n1 = p1.GetDirectorRef();
				auto& n2 = p2.GetDirectorRef();

				double dot = n1.dot(n2);

				return {-1.0*(_eps*(1.5*dot*dot - 0.5) + _gamma), 0.0};
			}

			double GetEpsilon() { return _eps; }
			double GetGamma() { return _gamma; }

	};
}
