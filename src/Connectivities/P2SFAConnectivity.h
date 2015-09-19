#pragma once 

#include "Connectivity.h"
#include "../Particles/Particle.h"
#include <functional>

namespace SAPHRON
{
	typedef std::function<void(const Particle&, Director&)> DirectorFunc;

	// Class representing P2 Legendre polynomial (uniaxial) spin anchoring on a particle. 
	// The Hamiltonian defines a penalty function -coeff*P2(dot(d,n)) where d is the director is
	// a user supplied function.
	class P2SFAConnectivity : public Connectivity
	{
		private:
			double _coeff;
			Director _dir;
			DirectorFunc _func;

		public:
			// Creates a new spin anchoring connectivity with an anchoring coefficient 
			// of "coeff". The director is calculated by a user supplied function "func" that 
			// takes in a particle pointer and a reference to an output director.
			P2SFAConnectivity(double coeff, DirectorFunc func) : 
			_coeff(coeff), _dir({0.0, 0.0, 0.0}), _func(func){}
			
			// Evaluate Hamiltonian.
			virtual double EvaluateEnergy(const Particle& p) override
			{
				auto& dir = p.GetDirectorRef();
				
				// Calculate dir based on user supplied func.
				_func(p, _dir);

				double dot = arma::dot(_dir, dir);
				return -1.0*_coeff*(1.5*dot*dot - 0.5);
			}
	};
}