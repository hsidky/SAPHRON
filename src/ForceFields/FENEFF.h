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

			virtual Interaction Evaluate(const Particle&, 
										 const Particle&, 
										 const Position& rij,
										 unsigned int) override
			{
				Interaction ep;
				auto rsq = fdot(rij, rij);

				if(rsq < _rmaxsq)
				{
					auto sr6 = _sigma3*_sigma3/(rsq*rsq*rsq);
					auto rrmax = 1.0 - rsq/_rmaxsq;
					ep.energy = -0.5*_kspring*_rmaxsq*log(rrmax) 
					+ 4.0*_epsilon*(sr6*sr6-sr6) + _epsilon;
					ep.virial = _kspring/rrmax + 24.0*_epsilon*(sr6-2.0*sr6*sr6)/rsq;
				}
				else
					ep.energy = 3E100;

				return ep;
			}

			// Serialize FENE.
			virtual void Serialize(Json::Value& json) const override
			{
				json["type"] = "FENE";
				json["epsilon"] = _epsilon;
				json["sigma"] = sqrt(_sigmasq);
				json["kspring"] = _kspring;
				json["rmax"] = sqrt(_rmaxsq);
			}
	};
}