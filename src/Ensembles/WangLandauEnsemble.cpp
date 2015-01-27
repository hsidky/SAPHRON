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
		Ensemble<T>(model), rand(3), hist(minE, maxE, binCount)
	{
		// Calculate initial energy.
		_energy = CalculateTotalEnergy();
	}

	// Performs one Monte-Carlo iteration. This is precisely one random draw
	// from the model (one function call to model->DrawSample()).
	template<typename T>
	void WangLandauEnsemble<T>::Sweep()
	{
		int j = 0;
		while(_flatness < 0.8)
		{
			this->RunLoggers();
			this->IncrementSweeps();
			for(int i = 0; i < this->model.GetSiteCount(); i++)
				Iterate();

			_flatness = hist.CalculateFlatness();

			if(j % 1000 == 0)
			{
				for(int i = 0; i < hist.GetBinCount(); i++)
					std::cout << hist.GetValue(i) << ",";
				std::cout << std::endl;
			}
			j++;
		}
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
		double newE = _energy + (currH - prevH)/(this->model.GetSiteCount());

		// Check probability
		if(AcceptanceProbability(_energy, newE) < rand.doub())
			for(auto &move : this->moves)
				move->Undo();
		else // Accepted
			_energy = newE;

		// Update bins and energy regardless (log DOS).
		auto bin = hist.Record(_energy);
		hist.UpdateValue(bin, hist.GetValue(bin)+log(GetScaleFactor()));
	}

	// Wang-Landau acceptance probability based on density of states.
	// Note that the "H" passed in here is the total energy of the system, rather than
	// individual energy (of a site). The probability is calculated as exp(log(gprev)-log(gcurr))
	// where log(g) is the associates DOS from the histogram.
	template<typename T>
	double WangLandauEnsemble<T>::AcceptanceProbability(double prevH, double currH)
	{
		auto p = exp(hist.GetValue(prevH) - hist.GetValue(currH));
		return p > 1.0 ? 1.0 : p;
	}

	// Calculates the total energy for the model.
	template<typename T>
	double WangLandauEnsemble<T>::CalculateTotalEnergy()
	{
		double u = 0;
		int c = this->model.GetSiteCount();
		for(int i = 0; i < c; i++)
			u += this->model.EvaluateHamiltonian(i);
		u /= 2.0*c;
		return u;
	}

	template<typename T>
	void WangLandauEnsemble<T>::RegisterLoggableProperties(Logger& logger)
	{
		logger.AddEnsembleProperty("Flatness", _flatness);
		logger.AddEnsembleProperty("Energy", _energy);
	}

	template class WangLandauEnsemble<Site>;
}
