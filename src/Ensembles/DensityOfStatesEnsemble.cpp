#include "DensityOfStatesEnsemble.h"

namespace Ensembles
{
	// Calculates the total energy of the model.
	template<typename T>
	double DensityOfStatesEnsemble<T>::CalculateTotalEnergy()
	{
		double u = 0;
		for(int i = 0; i < this->model.GetSiteCount(); i++)
			u += this->model.EvaluateHamiltonian(i);
		u /= 2.0;
		return u;
	}

	// Registers loggable properties with logger.
	template<typename T>
	void DensityOfStatesEnsemble<T>::RegisterLoggableProperties(DataLogger& logger)
	{
		logger.RegisterEnsembleProperty("Flatness", _flatness);
		logger.RegisterEnsembleProperty("Energy", _energy);
		logger.RegisterEnsembleProperty("ScaleFactor", _scaleFactor);
		logger.RegisterEnsembleProperty("Walker", _walker);
		logger.RegisterEnsembleVectorProperty("DOS", *_DOS);
		logger.RegisterEnsembleVectorProperty("Histogram", *_counts);
		logger.RegisterEnsembleProperty("LowerOutliers",
		                                _lowerOutliers);
		logger.RegisterEnsembleProperty("UpperOutliers",
		                                _upperOutliers);
	}

	// Runs multiple DOS simulations, between each subsequent iteration is
	// a re-normalization step involving resetting of the hisogram and reduction of
	// the scaling factor.
	template<typename T>
	void DensityOfStatesEnsemble<T>::Run(int iterations)
	{
		for(int i = 0; i < iterations; i++ )
		{
			Sweep();

			// Force logging
			this->RunLoggers(true);
			this->ResetHistogram();
			this->ReduceScaleFactor();
		}
	}

	// Performs one DOS iteration. This constitutes sampling phase space
	// until the density of states histogram is determined to be flat.
	// Performs one Monte-Carlo iteration. This is precisely one random draw
	// from the model (one function call to model->DrawSample()).
	template<typename T>
	void DensityOfStatesEnsemble<T>::Sweep()
	{
		_flatness = hist.CalculateFlatness();
		while(_flatness < GetTargetFlatness())
		{
			this->RunLoggers();
			this->IncrementSweeps();

			for(int i = 0; i < this->model.GetSiteCount(); i++)
				Iterate();

			_flatness = hist.CalculateFlatness();
			_lowerOutliers = hist.GetLowerOutlierCount();
			_upperOutliers = hist.GetUpperOutlierCount();
		}
	}

	template class DensityOfStatesEnsemble<Site>;
}
