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

		protected:

			// Increment iterations.
			inline void IncrementIterations()
			{
				++_iterations;
			}

			// Visit children.
			virtual void VisitChildren(Visitor& v) = 0;

		public:
			typedef std::map<std::string, double> AcceptanceMap;

			// Run the Ensemble simulation for a specified number of iterations.
			virtual void Run(int iterations) = 0;

			/* Getters and setters */

			// Get seed.
			virtual int GetSeed() const = 0;

			// Set seed.
			virtual void SetSeed(int seed) = 0;

			// Gets the current iteration count.
			inline int GetIteration() const { return _iterations; }

			// Reset iteration count.
			void ResetIterations()
			{
				_iterations = 0;
			}

			// Accept a visitor.
			virtual void AcceptVisitor(class Visitor &v)
			{
				v.Visit(this);
				VisitChildren(v);
			}

			/* Properties */
			virtual std::string GetName() const = 0;
			virtual AcceptanceMap GetAcceptanceRatio() const { return {}; }

			virtual ~Ensemble(){}
	};
}
