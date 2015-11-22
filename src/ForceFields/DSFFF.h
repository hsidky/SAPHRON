#pragma once 

#include "ForceField.h"
#include "../Particles/Particle.h"
#include "../Simulation/SimInfo.h"
#include "../Worlds/World.h"
#include <cmath>

namespace SAPHRON
{
	// Damped Shifted Force (DSF) pairwise electrostatic potential 
	// method for point charges as describe in
	// Fennell, C. J., & Gezelter, J. D. (2006). JCP, 124(23).
	// It is a real-space alternative to the Ewald-summation which has 
	// been shown to faithfully reproduce it in many environments.
	class DSFFF : public ForceField
	{
	private:
		// Damping parameter.
		double _alpha;
		double _alphasq;
		double _pre;
		double _qdim;
		std::vector<double> _erfcarc;
		std::vector<double> _erfexpsum;
		CutoffList _rc;
		
	public:
		DSFFF(double alpha, const CutoffList& rc) : 
		_alpha(alpha), _alphasq(alpha*alpha), _pre(2.0*_alpha/sqrt(M_PI)),
		_qdim(1.0), _erfcarc(0), _erfexpsum(0), _rc(rc)
		{
			// Calculate charge (q) reduced units conversion.
			auto& sim = SimInfo::Instance();
			_qdim = sim.GetChargeConv();

			// Precompute cutoff values.
			for(auto& r : _rc)
			{
				_erfcarc.push_back(erfc(_alpha*r)/r);
				_erfexpsum.push_back(erfc(_alpha*r)/(r*r)+ _pre*exp(-_alphasq*r*r)/r);
			}

		}

		virtual Interaction Evaluate(const Particle& p1,
									 const Particle& p2,
									 const Position& rij,
									 unsigned int wid) override
		{
			using arma::norm;
			Interaction ep;

			auto r = norm(rij);

			if (r > _rc[wid])
				return ep;

			auto rsq = r*r;
			auto q1 = p1.GetCharge();
			auto q2 = p2.GetCharge();
			auto erfcar = erfc(_alpha*r);

			ep.energy = _qdim*q1*q2*(
				erfcar/r - _erfcarc[wid] + _erfexpsum[wid]*(r-_rc[wid])
			);

			ep.virial = _qdim*q1*q2/r*(
				_erfexpsum[wid] - (erfcar/rsq + _pre*exp(-_alphasq*rsq)/r)
			);

			return ep;
		}

		// Serialize DSF.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "DSF";
			json["alpha"] = _alpha;
			for(auto& rc : _rc)
				json["rcut"].append(rc);
		}

		double GetAlpha() const { return _alpha; }
	};
}