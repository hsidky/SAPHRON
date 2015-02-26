#pragma once

#include "DensityOfStatesEnsemble.h"

namespace Ensembles
{
	// Class for Extended Ensemble Density-of-States (EXEDOS) sampling.
	template<typename T>
	class EXEDOSEnsemble : public DensityOfStatesEnsemble<T>
	{
		private:

			// Reduced temperature (kbT).
			double _temperature;

			// Number of species 1 sites.
			double _n1count = 0, _newn1count;

		protected:

			// Calculates the total number of a specified species.
			int CalculateSpeciesCount(int species = 1);

		public:

			// Initializes a EXEDOS sampler for a specified model at a given temperature (kbT).
			// The binning will be performed according to the specified minimum
			// and maxiumum mole fractions of species 1 in the system. Note that since
			// the there is a discrete number of lattice sites, a bincount larger than the
			// difference between the site counts will result in a histogram that never flattens.
			EXEDOSEnsemble(BaseModel& model,
			               double minN1,
			               double maxN1,
			               int binCount,
			               double temperature);

			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			void Iterate();

			// EXEDOS acceptance probability.
			double AcceptanceProbability(double prevH, double currH);

			// Gets the temperature (K).
			double GetTemperature()
			{
				return this->_temperature;
			}

			// Sets the temperature (K).
			double SetTemperature(double temperature)
			{
				return this->_temperature = temperature;
			}
	};
}
