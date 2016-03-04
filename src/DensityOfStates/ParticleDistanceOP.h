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
	class ParticleDistanceOP : public DOSOrderParameter
	{
	private:
		// Vector of particle groups.
		ParticleList _group1, _group2;

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

		// Initialize ParticleDistanceOP class. This will define an order parameter 
		// based on the distance between two groups of particles specified in the 
		// supplied particle lists.
		ParticleDistanceOP(
			const Histogram& hist, 
			const ParticleList& group1, 
			const ParticleList& group2) : 
		DOSOrderParameter(hist), _group1(group1), _group2(group2)
		{
		}

		double EvaluateOrderParameter(const World& w) const override
		{
			// Compute COM of each group.
			Position pos1{0,0,0}, pos2{0,0,0};
			auto m1 = 0., m2 = 0.;

			for(auto& p : _group1)
			{
				pos1 += p->GetPosition()*p->GetMass();
				m1 += p->GetMass();
			}
			pos1 /= m1;

			for(auto& p : _group2)
			{
				pos2 += p->GetPosition()*p->GetMass();
				m2 += p->GetMass();
			}
			pos2 /= m2;

			Position r = pos2 - pos1;
			w.ApplyMinimumImage(&r);
			return fnorm(r);
		}

		// Serialize.
		void Serialize(Json::Value& json) const override
		{
			json["type"] = "ParticleDistance";
			for(auto& p : _group1)
				json["group1"].append(p->GetGlobalIdentifier());
			for(auto& p : _group2)
				json["group2"].append(p->GetGlobalIdentifier());
		}

	};
}