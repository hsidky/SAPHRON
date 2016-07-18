#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include <armadillo>

namespace SAPHRON
{
	class PasquaMembraneC : public Constraint
	{
	private:
		double _eps, _d, _ra, _rb, _za, _zb, _neq, _npol;
		double _rasq, _rbsq, _zasq, _zbsq;
	public:
		PasquaMembraneC(
			World* world, 
			double eps, double d, double ra, 
			double rb, double za, double zb, 
			double neq, double npol
		) : Constraint(world), 
		_eps(eps), _d(d), _ra(ra), 
		_rb(rb), _za(za), _zb(zb), 
		_neq(neq), _npol(npol),
		_rasq(ra*ra), _rbsq(rb*rb), 
		_zasq(za*za), _zbsq(zb*zb)
		{
		}

		double EvaluateEnergy() const override
		{
			double Aeq = 0., Apol = 0., UHC = 0.;
			bool overlap = false;

			#pragma omp parallel for reduction(+:Aeq,Apol,UHC)
			for(int i = 0; i < _world->GetParticleCount(); ++i)
			{
				auto pi = _world->SelectParticle(i);
				double neq = 0., npol = 0.;
				for(auto& pj : pi->GetNeighbors())
				{
					Position rij = pi->GetPosition() - pj->GetPosition();
					auto r = fnorm(rij);
					auto rsq = r*r;
					auto zij = fdot(rij, pi->GetDirector())/r;
					auto zsq = zij*zij;

					// Hard core potential (limit).
					if(r <= _d)
					{
						UHC += 1e9;
						overlap = true;
					}
					else if(!overlap)
					{
						// Equation 4.
						auto Geq = 0.;
						if(r <= _ra)
							Geq = 1.;
						else if(_ra < r && r <= _rb)
							Geq = (_rbsq - rsq)/(_rbsq - _rasq);
						auto Gpol = Geq;

						// Equation 5.
						auto Heq = 0.;
						if(zsq <= _zasq)
							Heq = 1.;
						else if(_zasq < zsq && zsq <= _zbsq)
							Heq = (_zbsq - zsq)/(_zbsq - _zasq);
						auto Hpol = 1. - Heq;

						neq += Geq*Heq;
						npol += Gpol*Hpol;
					}
				}
				Aeq += (1. - neq/_neq)*(neq < _neq);
				Apol += (1. - npol/_npol)*(npol < _npol);
			}

			return UHC + _eps*(Aeq - Apol);
		}

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "PasquaMembrane";
			json["epsilon"] = _eps;
			json["d"] = _d;
			json["ra"] = _ra;
			json["rb"] = _rb;
			json["za"] = _za;
			json["zb"] = _zb;
			json["neq"] = _neq;
			json["npol"] = _npol;
		}
	};
}