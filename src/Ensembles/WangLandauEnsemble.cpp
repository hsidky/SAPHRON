#include "WangLandauEnsemble.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace Ensembles
{
	// Initializes a Wang-Landau sampler for a specified model. The binning will
	// be performed according to the specified minimum and maxiumum energies and
	// the bin count.
	template<typename T>
	WangLandauEnsemble<T>::WangLandauEnsemble(BaseModel& model,
	                                          double minE,
	                                          double maxE,
	                                          int binCount) :
		DensityOfStatesEnsemble<T>(model, minE*model.GetSiteCount(),
		                           maxE*model.GetSiteCount(), binCount)
	{
	}

	// Performs one Monte-Carlo iteration. This is precisely one random draw
	// from the model (one function call to model->DrawSample()).
	template<typename T>
	void WangLandauEnsemble<T>::Iterate()
	{
		// Draw sample and evaluate Hamiltonian.
		auto sample = this->model.DrawSample();
		double prevH = this->model.EvaluateHamiltonian(*sample);

		// Perform moves.
		for(auto &move : this->moves)
			move->Perform(*sample);

		// Get new hamiltonian. Since the Hamiltonian for a site is really the
		// change in energy of the system (in an isolated sense), we can just add
		// it to the current energy to get the absolute energy required for
		// AcceptanceProbability.
		double currH = this->model.EvaluateHamiltonian(*sample);
		double newE = this->_energy + (currH - prevH);

		// Check probability
		if(AcceptanceProbability(this->_energy, newE) < this->rand.doub())
			for(auto &move : this->moves)
				move->Undo();
		else // Accepted
			this->_energy = newE;

		// Update bins and energy regardless (log DOS).
		auto bin = this->hist.Record(this->_energy);
		this->hist.UpdateValue(bin, this->hist.GetValue(bin)+log(this->GetScaleFactor()));
	}

	// Wang-Landau acceptance probability based on density of states.
	// Note that the "H" passed in here is the total energy of the system, rather than
	// individual energy (of a site). The probability is calculated as exp(log(gprev)-log(gcurr))
	// where log(g) is the associates DOS from the histogram.
	template<typename T>
	double WangLandauEnsemble<T>::AcceptanceProbability(double prevH, double currH)
	{
		// If we are outside boundaries, drag the energy into the desired range.
		if(this->hist.GetBin(currH) == -1)
		{
			if(prevH < this->hist.GetMinimum() && currH > prevH)
				return 1;
			else if(prevH > this->hist.GetMaximum() && currH < prevH)
				return 1;

			return 0;
		}

		auto p = exp(this->hist.GetValue(prevH) - this->hist.GetValue(currH));
		return p > 1.0 ? 1.0 : p;
	}

	template class WangLandauEnsemble<Site>;
}
