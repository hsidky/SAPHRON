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
			this->NotifyObservers();
			this->IncrementSweeps();

			for(int i = 0; i < this->model.GetSiteCount(); i++)
				Iterate();

			_flatness = hist.CalculateFlatness();
		}
	}

	template class DensityOfStatesEnsemble<Site>;
}
