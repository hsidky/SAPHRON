#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class for FENE potential. See Kremer, Grest, J. Chem. Phys. 92, 5057 (1990)
	class FENEFF : public ForceField
	{
		private:
			double _epsilon;
			double _sigmasq;
			double _sigma3;

			//Spring constant for the spring
			double _kspring;

			//Maximum displacement of the spring
			double _rmaxsq;

		public:

			FENEFF(double epsilon, double sigma, double kspring, double rmax) : 
			_epsilon(epsilon), _sigmasq(sigma*sigma), _sigma3(_sigmasq*sigma), 
			_kspring(kspring), _rmaxsq(rmax*rmax)
			{ 
			}

			// TODO: virial for FENE
			inline virtual Interaction Evaluate(const Particle&, const Particle&, const Position& rij) override
			{
				Interaction ep;

				double rsq = rij.normsq();

				if(rsq < _rmaxsq)
				{
					double sr6 = _sigma3*_sigma3/(rsq*rsq*rsq);
					ep.energy = -0.5*_kspring*_rmaxsq*log(1-(rsq/_rmaxsq))+4.0*_epsilon*(sr6*sr6-sr6) + _epsilon;
				}

				//Hard potential, spring stretched too far
				//TODO: Change to a flag that will stop the simulation
				else
				{
					ep.energy = 3E100;
				}


				return ep;
			}

			double GetEpsilon() { return _epsilon; }
			double GetSigma() { return sqrt(_sigmasq); }
			double GetKSPring() { return _kspring; }
			double GetRMax() { return sqrt(_rmaxsq); }

	};
}