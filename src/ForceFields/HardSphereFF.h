#pragma once 

#include "ForceField.h"

namespace SAPHRON
{
	// Class for hard sphere potential.
	class HardSphereFF : public ForceField
	{
	private:
		double _sigma;
		double _sigmasq;

	public:
		HardSphereFF(double sigma): 
		_sigma(sigma), _sigmasq(sigma*sigma)
		{
		}

		virtual Interaction Evaluate(const Particle&, 
									 const Particle&, 
									 const Position& rij,
									 unsigned int) override
		{
			Interaction ep;

			if(fdot(rij, rij) > _sigmasq)
				ep.energy = 0;
			else
				ep.energy = 1e100;

			return ep;
		}

		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "HardSphere";
			json["sigma"] = _sigma;
		}	
	};
}