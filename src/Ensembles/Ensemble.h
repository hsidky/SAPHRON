#pragma once

#include "../Simulation/SimObservable.h"
#include "../Properties/Energy.h"
#include "../Properties/Pressure.h"
#include "../Worlds/World.h"
#include "json/json.h"
#include <map>
#include <vector>

namespace SAPHRON
{
	// Base class for simulation Ensembles. An ensemble is provided with a reference to a World and a
	// ForceFieldManager. The World is responsible for handling the "box" geometry and particles. The
	// ForeceFieldManager contains the forcefield data for all Particle types and interactions.
	class Ensemble : public SimObservable
	{
		private:
			// Iteration counter.
			int _iterations = 0;

			// Moves per iteration.
			int _mpi = 0;

		protected:

			// Increment iterations.
			inline void IncrementIterations()
			{
				++_iterations;
			}

			// Visit children.
			virtual void VisitChildren(Visitor& v) const = 0;

		public:
			typedef std::map<std::string, double> AcceptanceMap;

			Ensemble() : _iterations(0), _mpi(0) {}

			// Run the Ensemble simulation for a specified number of iterations.
			virtual void Run(int iterations) = 0;

			/* Getters and setters */

			// Get moves per iteration.
			int GetMovesPerIteration() const { return _mpi; }

			// Set moves per iteration.
			void SetMovesPerIteration(int mpi) { _mpi = mpi; }

			// Gets the current iteration count.
			inline int GetIteration() const { return _iterations; }

			// Reset iteration count.
			void ResetIterations()
			{
				_iterations = 0;
			}

			// Accept a visitor.
			virtual void AcceptVisitor(Visitor& v) const override
			{
				v.Visit(*this);
				VisitChildren(v);
			}

			/* Properties */
			virtual std::string GetName() const = 0;
			virtual AcceptanceMap GetAcceptanceRatio() const { return {}; }

			virtual ~Ensemble(){}
	};
}
