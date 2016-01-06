#pragma once 

#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class for truncated and shifted 12-6 Lennard Jones potential.
	class ModLennardJonesTSFF : public ForceField
	{
	private:
		double _epsilon;
		double _sigmasq;
		double _sigma3;
		double _beta;
		std::vector<double> _vrc; // Potential at cutoff.
		CutoffList _rcsq;
		CutoffList _rc;

	public:
		ModLennardJonesTSFF(double epsilon, double sigma, double beta, const CutoffList& rc) : 
		_epsilon(epsilon), _sigmasq(sigma*sigma), _sigma3(_sigmasq*sigma), _beta(beta),
		 _vrc(0), _rcsq(0), _rc(rc)
		{ 
			for(auto& r : _rc)
			{
				_vrc.push_back(
					4.0*epsilon*(pow(sigma/r, 12) - pow(sigma/r,6)) 
				);

				_rcsq.push_back(r*r);
			}
		}

		virtual Interaction Evaluate(const Particle&, 
									 const Particle&, 
									 const Position& rij,
									 unsigned int wid) override
		{
			Interaction ep;

			auto rsq = fdot(rij, rij);
			auto r = sqrt(rsq);
			auto rc = sqrt(_rcsq[wid]);
			if(rsq > _rcsq[wid])
				return ep;

			double sr6 = _sigma3*_sigma3/(rsq*rsq*rsq);
			
			ep.energy = 4.0*_epsilon*((sr6*sr6 - sr6) + 
			_beta*(cos(2.0*M_PI*r/rc)-1.0)) - _vrc[wid];

			ep.virial = 24.0*_epsilon*(sr6-2.0*sr6*sr6)/rsq - 
			sin(2.0*M_PI*r/rc)*8.0*_epsilon*_beta*M_PI/rc;
			
			return ep;
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{	
			json["type"] = "LennardJonesTS";
			json["sigma"] = sqrt(_sigmasq);
			json["epsilon"] = _epsilon;
			json["beta"] = _beta;
			for(auto& rc : _rc)
				json["rcut"].append(rc);
		}
	};
}