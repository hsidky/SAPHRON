#pragma once 

#include "Ensemble.h"
#include "../Worlds/World.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"

namespace SAPHRON
{
	typedef std::pair<double, double> Interval;

	class WLDOSEnsemble : public Ensemble
	{
		private: 

			// Energy interval.
			Interval _E;

		public:
			WLDOSEnsemble(World& world, 
						  ForceFieldManager& ffmanager, 
					      MoveManager& mmanager, Interval E, int seed = 1)
			{

			}

			~WLDOSEnsemble();
		
	};
}