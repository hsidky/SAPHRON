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

			// Flatness measure.
			double _flatness;

			// Densities of state pointer.
			std::vector<double>* _DOS;

			// Histogram counts.
			std::vector<double>* _counts;

			// Flatness threshold.
			double _targetFlatness = 0.8;

			double _lowerOutliers = 0;

			double _upperOutliers = 0;

		protected:
			// Calculates the total energy of the model.
			double CalculateTotalEnergy();

			void RegisterLoggableProperties(Logger& logger);

		public:
			// Initializes a Wang-Landau sampler for a specified model. The binning will
			// be performed according to the specified minimum and maxiumum energies and
			// the bin count.
			WangLandauEnsemble(BaseModel& model, double minE, double maxE, int binCount);

			// Runs multiple Wang-Landau simulations, between each subsequent iteration is
			// a re-normalization step involving resetting of the hisogram and reduction of
			// the scaling factor.
			void Run(int iterations);

			// Runs multiple Wang-Landau simulations until a specified tolerance is met.
			// Between each sweep the histogram is reset and the scaling factor is reduced.
			void Run(double tolerance);

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

			double GetTargetFlatness()
			{
				return _targetFlatness;
			}

			double SetTargetFlatness(double f)
			{
				return _targetFlatness = f;
			}

			// Resets the histogram.
			void ResetHistogram()
			{
				hist.ResetHistogram();
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
