#pragma once

#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/NewMove.h"
#include "../Worlds/World.h"
#include <vector>

namespace SAPHRON
{
	// Base class for simulation Ensembles. An ensemble is provided with a reference to a World and a
	// ForceFieldManager. The World is responsible for handling the "box" geometry and particles. The
	// ForeceFieldManager contains the forcefield data for all Particle types and interactions.
	class Ensemble
	{
		private:

			// Reference to world.
			World& _world;

			// Reference to force field manager.
			ForceFieldManager& _ffmanager;

			// Vector of moves.
			std::vector<Move*> moves;

		public:
			Ensemble(World& world, ForceFieldManager& ffmanager) :
				_world(world), _ffmanager(ffmanager){}

			// Add a move to the move queue.
			void PushMove(Move& move)
			{
				moves.push_back(&move);
			}

			// Pops a move from the end of the move queue.
			void PopMove()
			{
				moves.pop_back();
			}
	};
}
