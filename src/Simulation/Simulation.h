#pragma once

#include "../Simulation/SimObservable.h"
#include "../Properties/Energy.h"
#include "../Properties/Pressure.h"
#include "../Worlds/World.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../JSON/Serializable.h"
#include "json/json.h"
#include "vecmap.h"
#include <vector>

namespace SAPHRON
{
	// Base class for standard simulation. An simulation is provided with a reference to a World and a
	// ForceFieldManager. The World is responsible for handling the "box" geometry and particles. The
	// ForeceFieldManager contains the forcefield data for all Particle types and interactions.
	class Simulation : public SimObservable, public Serializable
	{
		private:
			// Iteration counter.
			int _iterations;

			// Moves per iteration.
			int _mpi ;

			// Target iterations.
			int _targetit;

		protected:

			// Increment iterations.
			inline void IncrementIterations()
			{
				++_iterations;
			}

			// Visit children.
			virtual void VisitChildren(Visitor& v) const = 0;

		public:
			typedef vecmap<std::string, double> AcceptanceMap;

			Simulation() : _iterations(0), _mpi(0), _targetit(0) {}

			// Run the simulation target iteration number of times.
			void Run() { Run(_targetit); }

			// Run the simulation for a specified number of iterations.
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

			// Set number of target iterations.
			void SetTargetIterations(int targetit) { _targetit = targetit; }

			// Get number of target iterations.
			int GetTargetIterations() const { return _targetit; }

			// Accept a visitor.
			virtual void AcceptVisitor(Visitor& v) const override
			{
				v.Visit(*this);
				VisitChildren(v);
			}

			/* Properties */
			virtual std::string GetName() const = 0;
			virtual AcceptanceMap GetAcceptanceRatio() const { return AcceptanceMap{}; }

			// Serialize
			virtual void Serialize(Json::Value& json) const override
			{
				auto& sim = SimInfo::Instance();
				json["units"] = (sim.GetUnits() == real) ? "real" : "reduced";
				json["simtype"] = GetName();
				json["iterations"] = GetTargetIterations();
				json["mpi"] = GetMovesPerIteration();

				SerializeObservers(json["observers"]);
			}

			/* Static Builder methods */

			// Builds a simulation from a JSON node, along with the
			// required dependencies. If a dependency is not needed/initialized, 
			// simply pass a nullptr. I
			static Simulation* BuildSimulation(const Json::Value& json,
											   WorldManager* wm, 
											   ForceFieldManager* ffm,
											   MoveManager* mm, 
											   DOSOrderParameter* dop,
											   Histogram* hist);
			virtual ~Simulation(){}
	};
}
