#pragma once 

#include "ForceField.h"

namespace SAPHRON 
{
	// Class representing fluctuating biological membrane 
	// potential from Pasqua et al. JCP 132, 154107 (2010).
	class PasquaMembraneFF : public ForceField
	{
	private:
		double _eps, _d, _ra, _rb, _za, _zb, _neq, _npol;
		double _rasq, _rbsq, _zasq, _zbsq;
	public:
		PasquaMembraneFF(double eps, double d, double ra, double rb, double za, double zb, double neq, double npol) : 
		_eps(eps), _d(d), _ra(ra), _rb(rb), _za(za), _zb(zb), _neq(neq), _npol(npol),
		_rasq(ra*ra), _rbsq(rb*rb), _zasq(za*za), _zbsq(zb*zb)
		{
		}

		Interaction Evaluate(const Particle& pi, 
							 const Particle& pj, 
							 const Position& rij, 
							 unsigned int wid) const override
		{
			Interaction ep;
			auto r = fnorm(rij);
			auto rsq = r*r;
			auto zij = fdot(rij, pi.GetDirector())/r;
			auto zsq = zij*zij;

			// Hard particle limit.
			if(r < _d)
				return {1e7, 0};

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

			// Equations 2 and 3. 
			auto neq = Geq*Heq;
			auto npol = Gpol*Hpol;

			// Equations 6 and 7.
			auto Aeq = 1. - neq/_neq*(neq < _neq);
			auto Apol = 1. - npol/_npol*(npol < _npol);

			return {_eps*(Aeq - Apol), 0};
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