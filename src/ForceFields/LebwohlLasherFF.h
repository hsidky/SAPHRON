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

			// Evaluate Hamiltonian.
			inline virtual double Evaluate(const Particle& p1, const Particle& p2) override
			{
				auto& n1 = p1.GetDirectorRef();
				auto& n2 = p2.GetDirectorRef();

				// Dot product
				double dot = 0;
				for(int i = 0; i < 3; i++)
					dot += n1[i]*n2[i];

				return -1.0*(_eps*(1.5*dot*dot - 0.5) + _gamma);
			}
	};
}
