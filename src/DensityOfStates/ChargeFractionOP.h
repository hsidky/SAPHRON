#pragma once 

#include "DOSOrderParameter.h"
#include "../Particles/Particle.h"
#include "../Simulation/SimInfo.h"
#include "../Properties/Energy.h"
#include "../Worlds/World.h"
#include "../Utils/Helpers.h"

namespace SAPHRON
{
	// Class for particle distance order parameter for two groups 
	// each containing one or more particle.
	class ChargeFractionOP : public DOSOrderParameter
	{
	private:
		// Vector of particle groups.
		ParticleList _group1;
		double _base_charge;

	protected:
		double CalcAcceptanceProbability(const Energy& ei,
										 const Energy& ef,
										 double opi, 
										 double opf, 
										 const World& w) const override
		{
			auto& sim = SimInfo::Instance();
			auto de = ef.total() - ei.total();
			auto dop = GetHistValue(opf) - GetHistValue(opi);

			auto p = exp(-de/(sim.GetkB()*w.GetTemperature()) - dop);
			return p > 1.0 ? 1.0 : p;
		}

	public:

		// Initialize ChargeFractionOP class. This will define an order parameter 
		// based on the charge fraction of the group of supplied particles
		ChargeFractionOP(
			const Histogram& hist, 
			const ParticleList& group1,
			double charge) : 
		DOSOrderParameter(hist), _group1(group1), _base_charge(charge)
		{
		}

		double EvaluateOrderParameter(const World& w) const override
		{
			
			auto ChargeFrac = 0.;

			for(auto& p : _group1)
			{
				ChargeFrac += p->GetCharge();
			}

			ChargeFrac /= _group1.size();
			ChargeFrac /= _base_charge;
			
			return fabs(ChargeFrac);
		}

		// Serialize.
		void Serialize(Json::Value& json) const override
		{
			json["type"] = "ChargeFraction";
			for(auto& p : _group1)
				json["group1"].append(p->GetGlobalIdentifier());
			json["Charge"] = _base_charge;
		}
	};
}