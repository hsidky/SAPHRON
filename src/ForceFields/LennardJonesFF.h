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
		public:

			LennardJonesFF(double epsilon, double sigma) : 
			_epsilon(epsilon), _sigmasq(sigma*sigma), _sigma3(_sigmasq*sigma)
			{ 
			}

			inline virtual Interaction Evaluate(const Particle&, 
												const Particle&, 
												const Position& rij,
												double) override
			{
				Interaction ep;

				double rsq = rij.normsq();
				double sr6 = _sigma3*_sigma3/(rsq*rsq*rsq);
				ep.energy = 4.0*_epsilon*(sr6*sr6-sr6);
				ep.virial = 24.0*_epsilon*(sr6-2.0*sr6*sr6)/rsq;
				
				return ep;
			}

			inline virtual double EnergyTailCorrection(double rcut) override
			{
				auto rcut3 = rcut*rcut*rcut;
				auto rcut9 = rcut3*rcut3*rcut3;
				return 4.0/3.0*_epsilon*_sigma3*(1.0/3.0*(_sigma3*_sigma3*_sigma3)/rcut9-_sigma3/rcut3); 
			}

			inline virtual double PressureTailCorrection(double rcut) override 
			{
				auto rcut3 = rcut*rcut*rcut;
				auto rcut9 = rcut3*rcut3*rcut3;
				return 8.0*_epsilon*_sigma3*(2.0/3.0*(_sigma3*_sigma3*_sigma3)/rcut9-_sigma3/rcut3); 
			}

			double GetEpsilon() { return _epsilon; }
			double GetSigma() { return sqrt(_sigmasq); }
	};
}