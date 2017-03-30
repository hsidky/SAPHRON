#pragma once 

#include "ForceField.h" 
#include "../Particles/Particle.h" 
#include "../Simulation/SimInfo.h" 
#include "../Worlds/World.h"
#include <cmath>

namespace SAPHRON 
{
	class EwaldFF : public ForceField
	{
	private: 
		double _alpha; 
		double _kmax;
		CutoffList _rc; 
		double _qdim;

	public:
		EwaldFF(double alpha, double kmax, const CutoffList& rc) : 
		_alpha(alpha), _kmax(kmax), _rc(rc)
		{
			auto& sim = SimInfo::Instance(); 
			_qdim = sim.GetChargeConv();         
		}

		Interaction Evaluate(const Particle& p1,
							const Particle& p2,
							const Position& rij,
							unsigned int wid) const override
		{
			Interaction ep; 
			auto r = fnorm(rij); 

			if(r > _rc[wid])
				return ep;

			auto q1 = p1.GetCharge();
			auto q2 = p2.GetCharge();
			auto erfcr = std::erfc(_alpha*r);
			ep.energy = _qdim*q1*q2*erfcr/r;

			if(p1.HasParent() && p2.HasParent() && p1.GetParent() == p2.GetParent())
				ep.energy -= _qdim*q1*q2*(1.-erfcr)/r;
			
			return ep;
		}

		double ReciprocalSpace(const World& w) const override
		{
			double u = 0;
			auto& H = w.GetHMatrix();

			for(int i = 0; i < w.GetPrimitiveCount(); ++i)
			{
				auto p = w.SelectPrimitive(i);
				auto q = p->GetCharge();
				u -= _alpha/std::sqrt(M_PI)*q*q;
			}

			double coeff = 0.5/(M_PI*w.GetVolume());
			for(int kx = -_kmax; kx < _kmax; ++kx)
				for(int ky = -_kmax; ky < _kmax; ++ky)
					for(int kz = -_kmax; kz < _kmax; ++kz)
					{
						if(kx == 0 && ky == 0 && kz == 0)
							continue; 
			
						double ksq = kx*kx + ky*ky + kz*kz;
						if(ksq > _kmax*_kmax + 2)
							continue;

						double hx = kx/H(0,0);
						double hy = ky/H(1,1); 
						double hz = kz/H(2,2);
						double hsq = hx*hx + hy*hy + hz*hz;

						double csum = 0, ssum = 0;
						for(int i = 0; i < w.GetPrimitiveCount(); ++i)
						{
							auto p = w.SelectPrimitive(i);
							auto q = p->GetCharge();
							auto& x = p->GetPosition();
							csum += q*std::cos(2.*M_PI*(x[0]*hx + x[1]*hy + x[2]*hz));
							ssum += q*std::sin(2.*M_PI*(x[0]*hx + x[1]*hy + x[2]*hz));
						}
						u += coeff/hsq*std::exp(-M_PI*M_PI*hsq/(_alpha*_alpha))*(csum*csum + ssum*ssum);
					}
			return _qdim*u;
		}


		void Serialize(Json::Value& json) const override
		{

		}
	};
}