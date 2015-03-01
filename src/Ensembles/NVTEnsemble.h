#pragma once

#include "../Models/BaseModel.h"
#include "../Rand.h"
#include "Ensemble.h"

using namespace Simulation;
using namespace Visitors;

namespace Ensembles
{
	// Class for simple NVT ensemble - also known as "canonical" ensemble. This
	// calls moves off the move queue and uses the Metropolis algorithm for acceptance
	// probability, A(i->j) = min(1,exp(DE/kB*T)). The template represents the return
	// type of DrawSample from model, which should typically be Site.
	template <typename T>
	class NVTEnsemble : public Ensemble<T>
	{
		private:

			// Temperature (K) (Sometimes reduced).
			double _temperature;

			// Energy of system.
			double _energy = 0.0;

			// "Normalized" Boltzmann constant.
			double _kb = 1.0;

			// Random number generator.
			Rand rand;

		public:
			// Initializes NVTEnsemble for a model at a given "reduced" temperature.
			NVTEnsemble(BaseModel& model, double temperature);

			// Performs one Monte Carlo sweep. This is defined as "n" iterations,
			// where "n" is the number of sites in a model.
			void Sweep();

			// Performs one Monte Carlo iteration. This is precicely one random
			// draw from the model (one function call to model->DrawSample()).
			void Iterate();

			// Metropolis acceptance probability of the system transitioning from prevH
			// to currH via exp(-(currH-prevH)/kb*T).
			double AcceptanceProbability(double prevH, double currH);

			// Gets the temperature (K).
			double GetTemperature()
			{
				return this->_temperature;
			}

			// Sets the temperature (K).
			double SetTemperature(double temperature)
			{
				return this->_temperature = temperature;
			}

			// Get the system energy.
			double GetEnergy()
			{
				return this->_energy;
			}

			// Gets the "normalized" Boltzmann constant (J/K).
			double GetBoltzmannConstant()
			{
				return this->_kb;
			}

			// Accept visitor to class.
			virtual void AcceptVisitor(class Visitor& v)
			{
				v.Visit(this);
				this->model.AcceptVisitor(v);
			}
	};
}
