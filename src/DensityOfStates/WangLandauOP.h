#pragma once

#include "DOSOrderParameter.h"
#include <cmath>

namespace SAPHRON
{
	// Wang-Landau Density-of-States (DOS) order parameter. Based on original WL algorithm.
	// [1] Wang, F., & Landau, D. P. (2001). Physical Review Letters, 86, 2050–2053.
	// [2] Wang, F., & Landau, D. P. (2001). Physical Review E, 64, 1–16.
	class WangLandauOP : public DOSOrderParameter
	{
		public:

			// Return the order parameter based on the energy.
			inline virtual double EvaluateParameter(double energy) override
			{
				return energy;
			}

			// Calculate the acceptance probability.
			inline virtual double AcceptanceProbability(double, double prevO, 
														double, double newO) override 
			{
				double p = exp(prevO - newO);
				return p > 1.0 ? 1.0 : p;
			}

			~WangLandauOP(){};	
	};
}