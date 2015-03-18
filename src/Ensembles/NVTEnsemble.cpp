// #include "NVTEnsemble.h"
//
// namespace Ensembles
// {
// 	// Initializes NVTEnsemble for a model at a given "reduced" temperature.
// 	template<typename T>
// 	NVTEnsemble<T>::NVTEnsemble(BaseModel& model, double temperature)
// 		: Ensemble<T>(model), _temperature(temperature), rand(1)
// 	{
// 		_energy = 0.0;
// 		for (int i = 0; i < this->model.GetSiteCount(); i++)
// 			_energy += this->model.EvaluateHamiltonian(i);
// 		_energy /= 2.0;
// 	}
//
// 	// Performs one Monte Carlo sweep. This is defined as "n" iterations,
// 	// where "n" is the number of sites in a model.
// 	template<typename T>
// 	void NVTEnsemble<T>::Sweep()
// 	{
// 		for (int i = 0; i < this->model.GetSiteCount(); i++)
// 			Iterate();
//
// 		this->NotifyObservers(SimEvent(this));
// 		this->IncrementSweeps();
// 	}
//
// 	// Performs one Monte Carlo iteration. This is precicely one random
// 	// draw from the model (one function call to model->DrawSample()).
// 	template<typename T>
// 	void NVTEnsemble<T>::Iterate()
// 	{
// 		// Draw sample and evaluate Hamiltonian
// 		auto sample = this->model.DrawSample();
// 		double prevH = this->model.EvaluateHamiltonian(*sample);
//
// 		// Perform moves
// 		for (auto &move : this->moves)
// 			move->Perform(*sample);
//
// 		// Get new Hamiltonian.
// 		double currH = this->model.EvaluateHamiltonian(*sample);
// 		double newE = this->_energy + (currH - prevH);
//
// 		// Check probabilities.
// 		if (AcceptanceProbability(this->_energy, newE) < rand.doub())
// 			for (auto &move : this->moves)
// 				move->Undo();
// 		else
// 			this->_energy = newE;
//
// 		this->IncrementIterations();
// 	}
//
// 	// Metropolis acceptance probability of the system transitioning from prevH
// 	// to currH via exp(-(currH-prevH)/kb*T).
// 	template<typename T>
// 	double NVTEnsemble<T>::AcceptanceProbability(double prevH, double currH)
// 	{
// 		auto p =
// 		        exp(-(currH -
// 		              prevH) / (GetBoltzmannConstant() * GetTemperature()));
//
// 		return p > 1.0 ? 1.0 : p;
// 	}
//
// 	template class NVTEnsemble<Site>;
// }
