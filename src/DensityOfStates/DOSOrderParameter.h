#pragma once 
#include "Histogram.h"

namespace SAPHRON
{
	// Forward declare.
	class World;
	struct Energy;

	class DOSOrderParameter
	{
	private: 
		Histogram const* _hist;

	protected:
		virtual double CalcAcceptanceProbability(const Energy& ei, 
												 const Energy& ef, 
												 double opi, 
												 double opf,
												 const World& w) const = 0;

	public:
		DOSOrderParameter(const Histogram& hist)
		{
			_hist = &hist;
		}
		// Return the order parameter based on the energy.
		virtual double EvaluateOrderParameter(const World& w) const = 0;

		// Evaluate acceptance probability based on energy difference 
		// and order parameter difference.
		double AcceptanceProbability(const Energy& ei, 
									 const Energy& ef, 
									 double opi, 
									 double opf,
									 const World& w) const
		{
			// If order parameter is out of bounds, but is going 
			// towards interval, drive it there, otherwise reject move
			// outright.
			if(_hist->GetBin(opf) == -1)
			{
				if(opi < _hist->GetMinimum() && opf > opi)
					return 1.0;
				else if(opi > _hist->GetMaximum() && opf < opi)
					return 1.0;

				return 0.0;
			}

			return CalcAcceptanceProbability(ei, ef, opi, opf, w);
		}
		
		virtual ~DOSOrderParameter(){}
	};
}