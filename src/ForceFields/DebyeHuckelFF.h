#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"
#include <math.h>

namespace SAPHRON
{
	// Class for FENE potential. See Kremer, Grest, J. Chem. Phys. 92, 5057 (1990)
	class DebyeHuckelFF : public ForceField
	{
		private:

			// Reduced Boltzmann * Temperature
			double _kbt;

			//Reduced Debye-Huckel screening length
			double _debye;

			//Reduced Bjerrum length
			double _bjerrum;

			//The cutoff
			double _rcutoff;

		public:

			DebyeHuckelFF(double kbt, double debye, double bjerrum, double rcutoff) : 
			_kbt(kbt), _debye(debye), _bjerrum(bjerrum), _rcutoff(rcutoff)
			{ 
			}

			inline virtual Interaction Evaluate(const Particle&, const Particle&, const Position& rij) override
			{
				Interaction ep;

				double r = rij.norm();

				if(r < _rcutoff)
				{

					ep.energy = _kbt*(_bjerrum/r)*exp(-r/_debye);
					ep.virial = _kbt*_bjerrum*((-exp(-r/_debye)/(r*r))-(exp(-r/_debye)/(_debye*r)));
				}

				
				return ep;
			}

			double GetBjerrum() { return _bjerrum; }
			double GetDebye() { return _debye; }
	};
}