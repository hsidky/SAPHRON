#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"
#include <math.h>

namespace SAPHRON
{
	// Class for Debye Huckel electrostatic interaction.
	class DebyeHuckelFF : public ForceField
	{
	private:
		double _kappa;
		double _qdim;
		CutoffList _rc;

	public:

		DebyeHuckelFF(double kappa, const CutoffList& rc) : 
		_kappa(kappa), _qdim(1.0), _rc(rc)
		{ 
			// Calculate charge (q) reduced units conversion.
			auto& sim = SimInfo::Instance();
			_qdim = sim.GetChargeConv();
		}

		virtual Interaction Evaluate(const Particle& p1, 
									 const Particle& p2, 
									 const Position& rij,
									 unsigned int wid) override
		{
			Interaction ep;

			auto r = fnorm(rij);
			if(r > _rc[wid])
				return ep;

			auto rsq = r*r;
			auto q1 = p1.GetCharge();
			auto q2 = p2.GetCharge();
			auto er = exp(-_kappa*r);

			ep.energy = _qdim*q1*q2/r*er;
			ep.virial = -_qdim*q1*q2*er*(1.0+_kappa*r)/(rsq*r);
			
			return ep;
		}

		// Serialize DebyeHuckel.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "DebyeHuckel";
			json["kappa"] = _kappa;
		}
	};
}