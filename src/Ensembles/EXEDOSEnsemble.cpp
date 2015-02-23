#include "EXEDOSEnsemble.h"

namespace Ensembles
{
	// Initializes a EXEDOS sampler for a specified model at a given temperature (kbT).
	// The binning will be performed according to the specified minimum
	// and maxiumum mole fractions of species 1 in the system. Note that since
	// the there is a discrete number of lattice sites, a bincount larger than the
	// difference between the site counts will result in a histogram that never flattens.
	template<typename T>
	EXEDOSEnsemble<T>::EXEDOSEnsemble(BaseModel& model,
	                                  double minN1,
	                                  double maxN1,
	                                  int binCount,
	                                  double temperature) :
		DensityOfStatesEnsemble<T>(model, round(minN1*model.GetSiteCount()),
		                           round(maxN1*model.GetSiteCount()), binCount),
		_temperature(temperature)
	{
		_n1count = CalculateSpeciesCount(1);
	}

	// Performs one Monte-Carlo iteration. This is precisely one random draw
	// from the model (one function call to model->DrawSample()).
	template<typename T>
	void EXEDOSEnsemble<T>::Iterate()
	{
		// Draw sample and evaluate Hamiltonian.
		auto sample = this->model.DrawSample();

		int prevS = sample->GetSpecies();
		double prevH = this->model.EvaluateHamiltonian(*sample);

		// Select a move randomly.
		int moven = this->rand.int32() % this->moves.size();
		this->moves[moven]->Perform(*sample);

		// Get new hamiltonian. We can just add it to the current energy to
		// get the absolute energy required for AcceptanceProbability.
		double currH = this->model.EvaluateHamiltonian(*sample);
		int currS = sample->GetSpecies();

		// Calculate new energy and species count.
		double newE = this->_energy + (currH - prevH);
		if(prevS == 1 && currS != 1)
			_newn1count = _n1count - 1;
		else if(prevS != 1 && currS == 1)
			_newn1count = _n1count + 1;
		else
			_newn1count = _n1count;

		// Check probability.
		if(AcceptanceProbability(this->_energy, newE) < this->rand.doub())
			this->moves[moven]->Undo();
		else // Accepted
		{
			this->_energy = newE;
			this->_n1count = _newn1count;
		}

		// Update bins and energy regardless (log DOS).
		auto bin = this->hist.Record(this->_n1count);
		this->hist.UpdateValue(bin, this->hist.GetValue(bin)+this->GetScaleFactor());
	}

	// Calculates the total number of a specified species in the model.
	template<typename T>
	int EXEDOSEnsemble<T>::CalculateSpeciesCount(int species)
	{
		int counter = 0;
		for(int i = 0; i < this->model.GetSiteCount(); i++)
		{
			auto site = this->model.SelectSite(i);
			if(site->GetSpecies() == species)
				counter++;
		}

		return counter;
	}

	template<typename T>
	double EXEDOSEnsemble<T>::AcceptanceProbability(double prevH, double currH)
	{
		// If we are outside the boundaries, drag the n1 count into desired range.
		if(this->hist.GetBin(_newn1count) == -1)
		{
			if(_n1count < this->hist.GetMinimum() && _newn1count > _n1count)
				return 1;
			else if(_n1count > this->hist.GetMaximum() && _newn1count < _n1count)
				return 1;

			return 0;
		}

		auto p =
		        exp(-(currH-
		              prevH)/this->GetTemperature() +
		            (this->hist.GetValue(_n1count) - this->hist.GetValue(_newn1count)));
		return p > 1.0 ? 1.0 : p;
	}

	template<typename T>
	void EXEDOSEnsemble<T>::RegisterLoggableProperties(DataLogger &logger)
	{
		// Call parent.
		DensityOfStatesEnsemble<T>::RegisterLoggableProperties(logger);
		logger.RegisterEnsembleProperty("SpeciesCount", _n1count);
	}

	template class EXEDOSEnsemble<Site>;
}
