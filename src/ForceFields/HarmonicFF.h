#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class for FENE potential. See Kremer, Grest, J. Chem. Phys. 92, 5057 (1990)
	class Harmonic : public ForceField
	{
		private:

			//Spring constant for the spring
			double _kspring;

			//Equilibrium bond distance
			double _ro;

		public:

			Harmonic(double kspring, double ro) : _kspring(kspring), _ro(ro)
			{ 
			}

			virtual Interaction Evaluate(const Particle&, 
										 const Particle&, 
										 const Position& rij,
										 unsigned int) const override
			{
				Interaction ep;
				
				auto rsq = fdot(rij, rij);
				auto r = sqrt(rsq);

				ep.energy = 0.5*_kspring*(r-_ro)*(r-_ro);
				ep.virial = _kspring*(r-_ro);

				return ep;
			}

			// Serialize Harmonic.
			virtual void Serialize(Json::Value& json) const override
			{
				json["type"] = "Harmonic";
				json["kspring"] = _kspring;
				json["ro"] = sqrt(_ro);
			}
	};
}