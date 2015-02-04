#pragma once

#include "../Histogram.h"
#include "../Loggers/Logger.h"
#include "../Models/BaseModel.h"
#include "../Rand.h"
#include "Ensemble.h"
#include <vector>

namespace Ensembles
{
	// Abstract class for density of states based sampling using flat histogram
	// method.
	template<typename T>
	class DensityOfStatesEnsemble : public Ensemble<T>
	{
		private:

			// Flatness threshold.
			double _targetFlatness = 0.8;

			// Densities of state pointer.
			std::vector<double>* _DOS;

			// Histogram counts.
			std::vector<double>* _counts;

			// Current order parameter of the model.
			double _param;

			// Flatness measure.
			double _flatness;

			// Lower outliers.
			double _lowerOutliers = 0;

			// Upper outliers.
			double _upperOutliers = 0;

			// Scaling factor for density of states.
			double _scaleFactor = exp(1);

		protected:
			// Random number generator.
			Rand rand;

			// Histogram
			Histogram hist;

			// Current energy of the model.
			double _energy;

			// Calculates the total energy of the model.
			double CalculateTotalEnergy();

			// Registers loggable properties with logger.
			void RegisterLoggableProperties(Logger& logger);

		public:

			// Initializes a DOS ensemble for a specified model. The binning will
			// be performed according to the specified minimum and maxiumum parameter and
			// the bin count.
			DensityOfStatesEnsemble(BaseModel& model,
			                        double minP,
			                        double maxP,
			                        int binCount) :
				Ensemble<T>(model), rand(15), hist(minP, maxP, binCount)
			{
				_counts = hist.GetHistogramPointer();
				_DOS = hist.GetValuesPointer();

				// Calculate initial energy.
				_energy = CalculateTotalEnergy();
			};

			// Runs multiple DOS simulations, between each subsequent iteration is
			// a re-normalization step involving resetting of the hisogram and reduction of
			// the scaling factor.
			void Run(int iterations);

			// Performs one DOS iteration. This constitutes sampling phase space
			// until the density of states histogram is determined to be flat.
			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			void Sweep();

			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			virtual void Iterate() = 0;

			// Gets the current energy of the system.
			double GetEnergy()
			{
				return _energy;
			}

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

			// Reduces the scaling factor by a specified multiple.
			double ReduceScaleFactor(double multiple = 1, double power = 0.5)
			{
				return _scaleFactor = multiple*pow(_scaleFactor, power);
			}

			// Acceptance probability based on density of states.
			virtual double AcceptanceProbability(double prevH, double currH) = 0;
	};
}
