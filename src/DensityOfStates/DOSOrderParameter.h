#pragma once 
#include "Utils/Histogram.h"
#include "json/json.h"

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

		double GetHistValue(double op) const { return _hist->GetValue(op); }

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

		// Builds a density-of-states order parameter from JSON node. 
		// Requires histogram and world manager which contain dependencies 
		// for some/all order parameters. Returns pointer to OP. Throws 
		// BuildException upon failure. If nullptr is returned, unknown 
		// error occurred. Object lifetime is caller's responsibility!
		static DOSOrderParameter* Build(const Json::Value& json, 
										Histogram* hist,
										WorldManager* wm);
		
		virtual ~DOSOrderParameter(){}
	};
}