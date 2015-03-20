#pragma once

#include "../Simulation/SimObservable.h"
#include <vector>

namespace SAPHRON
{
	// Base class for simulation Ensembles. An ensemble is provided with a reference to a World and a
	// ForceFieldManager. The World is responsible for handling the "box" geometry and particles. The
	// ForeceFieldManager contains the forcefield data for all Particle types and interactions.
	class Ensemble : public SimObservable
	{
		private:

			// Boltzmann constant.
			double _kb = 1.0;

			// Iteration counter.
			int _iterations = 0;

		protected:

			// Increment iterations.
			inline void IncrementIterations()
			{
				++_iterations;
			}

		public:

			// Run the Ensemble simulation for a specified number of iterations.
			virtual void Run(int iterations) = 0;

			// Sets the Boltzmann constant.
			void SetBoltzmannConstant(double kb)
			{
				_kb = kb;
			}

			// Gets the Boltzmann constant.
			inline double GetBoltzmannConstant()
			{
				return _kb;
			}

			// Gets the current iteration count.
			inline int GetIteration()
			{
				return _iterations;
			}

			// Gets the number of iterations in the current simulation.
			virtual int GetSimIteration() override
			{
				return _iterations;
			}

			// Get the identifier of the observable.
			virtual int GetObservableID() override
			{
				return 0;
			}

			// Reset iteration count.
			void ResetIterations()
			{
				_iterations = 0;
			}

			// Accept a visitor.
			virtual void AcceptVisitor(class Visitor &v) override
			{
				v.Visit(this);
			}

			/* Properties */
			virtual double GetTemperature() { return 0.0; }

			virtual double GetEnergy() { return 0.0; }
	};
}
