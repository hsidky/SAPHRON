#pragma once

#include "../Histogram.h"
#include "../Loggers/Logger.h"
#include "../Models/BaseModel.h"
#include "../Rand.h"
#include "Ensemble.h"
#include <vector>

namespace Ensembles
{
	// Class for Wang-Landau density of states sampling.
	template<typename T>
	class WangLandauEnsemble : public Ensemble<T>
	{
		private:

			// Scaling factor for density of states.
			double _scaleFactor = exp(1);

			// Current energy of the model.
			double _energy;

			// Random number generator.
			Rand rand;

			// Histogram
			Histogram hist;

			double _flatness;

		protected:
			// Calculates the total energy of the model.
			double CalculateTotalEnergy();

			void RegisterLoggableProperties(Logger& logger);

		public:
			// Initializes a Wang-Landau sampler for a specified model. The binning will
			// be performed according to the specified minimum and maxiumum energies and
			// the bin count.
			WangLandauEnsemble(BaseModel& model, double minE, double maxE, int binCount);

			// Performs one Wang-Landau iteration. This constitutes sampling phase space
			// until the density of states histogram is determined to be flat.
			void Sweep();

			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			void Iterate();

			// Gets the density of states scaling factor.
			double GetScaleFactor()
			{
				return _scaleFactor;
			}

			// Sets the density of states scaling factor.
			double SetScaleFactor(double sf)
			{
				return _scaleFactor = sf;
			}

			// Gets the current energy of the system.
			double GetEnergy()
			{
				return _energy;
			}

			// Reduces the scaling factor by a specified multiple.
			virtual double ReduceScaleFactor(double multiple = 1, double power = 0.5)
			{
				return _scaleFactor = multiple*pow(_scaleFactor, power);
			}

			// Wang-Landau acceptance probability based on density of states.
			// Note that the "H" passed in here is the total energy of the system, rather than
			// individual energy (of a site).
			double AcceptanceProbability(double prevH, double currH);
	};
}
