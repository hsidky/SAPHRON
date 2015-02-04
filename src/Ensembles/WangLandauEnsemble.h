#pragma once

#include "DensityOfStatesEnsemble.h"

namespace Ensembles
{
	// Class for Wang-Landau density of states sampling.
	template<typename T>
	class WangLandauEnsemble : public DensityOfStatesEnsemble<T>
	{
		public:
			// Initializes a Wang-Landau sampler for a specified model. The binning will
			// be performed according to the specified minimum and maxiumum energies and
			// the bin count.
			WangLandauEnsemble(BaseModel& model, double minE, double maxE, int binCount);

			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			void Iterate();

			// Wang-Landau acceptance probability based on density of states.
			// Note that the "H" passed in here is the total energy of the system, rather than
			// individual energy (of a site).
			double AcceptanceProbability(double prevH, double currH);
	};
}
