#pragma once

#include "DOSOrderParameter.h"
#include "../Worlds/World.h"
#include <cmath>

namespace SAPHRON
{
	// Wang-Landau Density-of-States (DOS) order parameter. Based on original WL algorithm.
	// [1] Wang, F., & Landau, D. P. (2001). Physical Review Letters, 86, 2050–2053.
	// [2] Wang, F., & Landau, D. P. (2001). Physical Review E, 64, 1–16.
	class WangLandauOP : public DOSOrderParameter
	{
	protected:

		// Calculate Wang-Landau acceptance probability.
		virtual double CalcAcceptanceProbability(const Energy&, 
												 const Energy&, 
												 double opi, 
												 double opf,
												 const World&) const override
		{
			double p = exp(GetHistValue(opi) - GetHistValue(opf));
			return p > 1.0 ? 1.0 : p;
		}

	public:
		WangLandauOP(const Histogram& hist) : DOSOrderParameter(hist) {}

		// Return the Wang-Landau order parameter (energy).
		virtual double EvaluateOrderParameter(const World& w) const override
		{
			return w.GetEnergy().total();
		}

		~WangLandauOP(){};	
	};
}