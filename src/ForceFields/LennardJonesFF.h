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

			virtual double Evaluate(const Particle& p1, const Particle& p2) override
			{
				double E = 0;
				for(auto& neighbor : p1.GetNeighbors())
				{
					double rij = (p1.GetPositionRef() - neighbor->GetPositionRef()).norm();
					if(rij <= _rcut)
					{
						double r6 = pow(_sigma/rij,6);
						E += 4*_epsilon*(r6*r6-r6);
					}
				}

				return E;
			}
	};
}