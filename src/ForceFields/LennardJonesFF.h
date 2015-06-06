#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class for classic 12-6 Lennard Jones potential.
	class LennardJonesFF : public ForceField
	{
		private:
			double _epsilon;
			double _sigmasq;
			double _sigma3;
			double _rcutsq;
			double _rcut3;

		public:

			LennardJonesFF(double epsilon, double sigma, double rcut) : 
			_epsilon(epsilon), _sigmasq(sigma*sigma), _sigma3(_sigmasq*sigma), 
			_rcutsq(rcut*rcut), _rcut3(_rcutsq*rcut)
			{ 
			}

			inline virtual Interaction Evaluate(const Particle& p1, const Particle& p2) override
			{
				Interaction ep;

				Position rij =  p1.GetPositionRef() - p2.GetPositionRef();
				World* world = p1.GetWorld();
				if(world != nullptr)
					world->ApplyMinimumImage(rij);

				double rsq = rij.norm2();

				if(rsq < _rcutsq)
				{
					double sr6 = pow(_sigmasq/rsq,3);
					ep.energy = 4*_epsilon*(sr6*sr6-sr6);
					ep.virial = 24*_epsilon*(sr6-2*sr6*sr6)/rsq;
				}

				return ep;
			}

			inline virtual double EnergyTailCorrection() override
			{
				return 4.0/3.0*_epsilon*_sigma3*(1.0/3.0*pow(_sigma3,3)/pow(_rcut3,3)-_sigma3/_rcut3); 
			}

			inline virtual double PressureTailCorrection() override 
			{
				return 8.0*_epsilon*_sigma3*(2.0/3.0*pow(_sigma3,3)/pow(_rcut3,3)-_sigma3/_rcut3); 
			}

	};
}