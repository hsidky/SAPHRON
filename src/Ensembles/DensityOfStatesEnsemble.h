#pragma once

#include "../Histogram.h"
#include "../Models/BaseModel.h"
#include "../Rand.h"
#include "Ensemble.h"
#include <iomanip>
#include <vector>

namespace Ensembles
{
	typedef std::pair<double, double> Interval;

	// Abstract class for density of states based sampling using flat histogram
	// method.
	template<typename T>
	class DensityOfStatesEnsemble : public Ensemble<T>
	{
		private:

			// Flatness threshold.
			double _targetFlatness = 0.8;

			// Flatness measure.
			double _flatness;

			// Log of scaling factor for density of states.
			double _scaleFactor = 1;

			// Parameter histogram interval.
			Interval _interval;

			// Unique identifier of walker number.
			int _walker = 0;

		protected:
			// Random number generator.
			Rand rand;

			// Histogram
			Histogram hist;

			// Current energy of the model.
			double _energy;

			// Calculates the total energy of the model.
			double CalculateTotalEnergy();

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
				_interval = Interval(minP, maxP);

				// Calculate initial energy.
				_energy = CalculateTotalEnergy();

				std::cout << "Density of States Ensemble Initialized." << std::endl;
				std::cout << "Interval: " << std::setw(13) << std::left << minP;
				std::cout << std::setw(13) << std::right << maxP << std::endl <<
				std::endl;
			};

			// Runs multiple DOS simulations, between each subsequent iteration is
			// a re-normalization step involving resetting of the hisogram and reduction of
			// the scaling factor.
			virtual void Run(int iterations);

			// Performs one DOS iteration. This constitutes sampling phase space
			// until the density of states histogram is determined to be flat.
			// Performs one Monte-Carlo iteration. This is precisely one random draw
			// from the model (one function call to model->DrawSample()).
			virtual void Sweep();

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
			virtual double SetScaleFactor(double sf)
			{
				return _scaleFactor = sf;
			}

			// Gets current flatness.
			double GetFlatness()
			{
				return _flatness;
			}

			// Get Target flatness.
			virtual double GetTargetFlatness()
			{
				return _targetFlatness;
			}

			// Set target flatness.
			virtual double SetTargetFlatness(double f)
			{
				return _targetFlatness = f;
			}

			// Get Lower outlier count.
			int GetLowerOutlierCount()
			{
				return hist.GetLowerOutlierCount();
			}

			// Get upper outlier count.
			int GetUpperOutlierCount()
			{
				return hist.GetUpperOutlierCount();
			};

			// Get density of states vector (pointer).
			std::vector<double>* GetDensityOfStates()
			{
				return hist.GetValuesPointer();
			}

			// Resets the histogram.
			void ResetHistogram()
			{
				hist.ResetHistogram();
			}

			// Gets the interval over which the DOS is computed.
			Interval GetParameterInterval()
			{
				return _interval;
			}

			// Reduces the scaling factor order by a specified multiple.
			double ReduceScaleFactor(double order = 0.5)
			{
				// We store log of scale factor. So we simply multiply.
				return _scaleFactor = _scaleFactor*order;
			}

			// Gets the walker ID.
			int GetWalkerID()
			{
				return _walker;
			}

			// Sets the walker ID.
			int SetWalkerID(int id)
			{
				return _walker = id;
			}

			// Acceptance probability based on density of states.
			virtual double AcceptanceProbability(double prevH, double currH) = 0;
	};
}
