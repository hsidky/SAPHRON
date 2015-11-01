#pragma once 

#include "ForceField.h"
#include "../Particles/Particle.h"
#include "../Simulation/SimInfo.h"
#include <cmath>

namespace SAPHRON
{
	// Damped Shifted Force (DSF) pairwise electrostatic potential 
	// method for point charges as describe in
	// Fennell, C. J., & Gezelter, J. D. (2006). JCP, 124(23).
	// It is a real-space alternative to the Ewald-summation which has 
	// been shown to faithfully reproduce it.
	class DSFFF : public ForceField
	{
	private:
		// Damping parameter.
		double _alpha;
		double _alphasq;
		double _pre;
		double _qdim;
	public:
		DSFFF(double alpha) : 
		_alpha(alpha), _alphasq(alpha*alpha), _pre(2.0*_alpha/sqrt(M_PI)),
		_qdim(1.0)
		{
			// Calculate charge (q) reduced units conversion.
			auto& sim = SimInfo::Instance();
			_qdim = sim.GetChargeConv();
		}

		virtual Interaction Evaluate(const Particle& p1,
									 const Particle& p2,
									 const Position& rij,
									 double rc) override
		{
			using arma::norm;

			auto r = norm(rij);
			auto rsq = r*r;
			auto rcsq = rc*rc;
			auto q1 = p1.GetCharge();
			auto q2 = p2.GetCharge();
			auto erfcar = erfc(_alpha*r);
			auto erfcarc = erfc(_alpha*rc);
			auto exparcsq = exp(-_alphasq*rcsq);

			Interaction ep;
			ep.energy = _qdim*q1*q2*(
				erfcar/r - erfcarc/rc + (erfcarc/rcsq + _pre*exparcsq/rc)*(r-rc)
			);

			ep.virial = _qdim*q1*q2/r*(
				(erfcarc/rcsq + _pre*exparcsq/rc) - (erfcar/rsq + _pre*exp(-_alphasq*rsq)/r)
			);

			return ep;
		}

		// Serialize DSF.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "DSF";
			json["alpha"] = _alpha;
		}

		double GetAlpha() const { return _alpha; }
	};
}