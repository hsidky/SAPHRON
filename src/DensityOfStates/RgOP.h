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
	class RgOP : public DOSOrderParameter
	{
	private:
		// Vector of particle groups.
		ParticleList _group1;

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

		// Initialize RgOP class. This will define an order parameter 
		// based on the Rg of the group of supplied particles
		RgOP(
			const Histogram& hist, 
			const ParticleList& group1) : 
		DOSOrderParameter(hist), _group1(group1)
		{
		}

		double EvaluateOrderParameter(const World& w) const override
		{
			// Compute COM of each group.
			Position pos1{0,0,0};
			Position periodic{0,0,0}; 
			auto m1 = 0.;

			for(auto& p : _group1)
			{
				pos1 += p->GetPosition()*p->GetMass();
				m1 += p->GetMass();
			}
			pos1 /= m1;

			double Rg = 0;

			for(auto& p : _group1)
			{
				periodic = p->GetPosition() - pos1;
				w.ApplyMinimumImage(&periodic);
				Rg += p->GetMass()*fdot(periodic,periodic);
			}

			Rg = Rg/m1;

			return sqrt(Rg);

		}

		// Serialize.
		void Serialize(Json::Value& json) const override
		{
			json["type"] = "Rg";
			for(auto& p : _group1)
				json["group1"].append(p->GetGlobalIdentifier());
		}

	};
}