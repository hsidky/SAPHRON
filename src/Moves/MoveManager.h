#pragma once 

#include "NewMove.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Class for managing moves.
	class MoveManager
	{
	private:
		Rand _rand;
		std::vector<Move*> _moves;

	public:
		MoveManager(int seed = 1) : _rand(1){}

		// Add a move to the move queue.
		void PushMove(Move& move)
		{
			_moves.push_back(&move);
		}

		// Pops a move from the end of the move queue.
		void PopMove()
		{
			_moves.pop_back();
		}
	};
}