#pragma once

#include "../Rand.h"
#include "NewMove.h"

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

			// Select a move by index.
			Move* SelectMove(int i)
			{
				return _moves[i];
			}

			// Get the number of moves.
			int GetMoveCount()
			{
				return (int)_moves.size();
			}

			// Select a random move.
			inline Move* SelectRandomMove()
			{
				return _moves[_rand.int32() % _moves.size()];
			}
	};
}
