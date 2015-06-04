#pragma once 

#include "ForceField.h"

namespace SAPHRON
{
	// Class for classic 12-6 Lennard Jones potential.
	class LennardJonesFF : public ForceField
	{
		private:
			double _epsilon;
			double _sigma;
			double _rcut;

			LennardJonesFF(double epsilon, double sigma, double rcut) : 
				_epsilon(epsilon), _sigma(sigma), _rcut(rcut)
			{

			}

			inline virtual Interaction Evaluate(const Particle& p1, const Particle& p2) override
			{
				Interaction ep;

				double rij = (p1.GetPositionRef() - p2.GetPositionRef()).norm();
				if(rij < _rcut)
				{
					double sr6 = pow(_sigma/rij,6);
					ep.energy += 4*_epsilon*(sr6*sr6-sr6);
					ep.virial += 24*_epsilon*(sr6-2*sr6*sr6)/rij;
				}

				return ep;
			}

	};
}