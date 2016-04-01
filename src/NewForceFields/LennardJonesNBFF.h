#pragma once 

#include "../Utils/Helpers.h"
#include "NonBondedFF.h"

namespace SAPHRON
{
	// Class for classic 12-6 Lennard Jones potential.
	class LennardJonesNBFF : public NonBondedFF
	{
	private:
		double eps_;
		double sigsq_;
		double sig6_;
		std::vector<double> rcsq_;
		std::vector<double> rc_;
		std::vector<double> etail_;
		std::vector<double> ptail_;

	public:
		LennardJonesNBFF(double eps, double sig, const std::vector<double>& rc) :
		eps_(eps), sigsq_(fsq(sig)), sig6_(fpow6(sig)), rcsq_(0), rc_(rc),
		etail_(0), ptail_(0)
		{
			for(auto& r : rc_)
			{
				auto r3 = fcb(r);
				auto r9 = fcb(r3);
				auto sig3 = fcb(sig);
				etail_.push_back(
					4./3.*eps_*sig3*(1./3.*fcb(sig3)/r9 - sig3/r3)
				);

				ptail_.push_back(
					8.*eps_*sig3*(2./3.*fcb(sig3)/r9 - sig3/r3)
				);

				rcsq_.push_back(fsq(r));
			}
		}

		double EvaluateEnergy(const Site&, const Site&, const Vector3&, double rsq, uint wid) const override
		{
			if(rsq > rcsq_[wid])
				return 0.;
			
			auto sr6 = sig6_/(rsq*rsq*rsq);
			auto sr12 = sr6*sr6;
			return 4.*eps_*(sr12 - sr6);
		}

		Vector3 EvaluateForce(const Site&, const Site&, const Vector3& rij, double rsq, uint wid) const override
		{
			if(rsq > rcsq_[wid])
				return {0., 0., 0.};

			auto sr6 = sig6_/(rsq*rsq*rsq);
			auto sr12 = sr6*sr6;
			return -24.*eps_*(2.*sr12 - sr6)/rsq*rij;
		}

		double EnergyTailCorrection(uint wid) const override
		{
			return etail_[wid];
		}

		double PressureTailCorrection(uint wid) const override
		{
			return ptail_[wid];
		}
	};
}