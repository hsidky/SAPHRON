#pragma once

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Class for managing moves.
	class MoveManager
	{
		private:
			typedef std::vector<Move*> MoveList;
			Rand _rand;
			MoveList _moves;

		public:
			typedef MoveList::iterator iterator;
			typedef MoveList::const_iterator const_iterator;

			MoveManager(int seed = 1) : _rand(seed){}

			// Add a move to the move queue.
			void PushMove(Move& move)
			{
				_moves.push_back(&move);
			}

			// Add a move to the move queue.
			void PushMove(Move* move)
			{
				_moves.push_back(move);
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

			// Reset acceptance ratio on moves.
			void ResetMoveAcceptances()
			{
				for(auto& move : _moves)
					move->ResetAcceptanceRatio();
			}

			// Select a random move.
			inline Move* SelectRandomMove()
			{
				return _moves[_rand.int32() % _moves.size()];
			}

			// Seed seed.
			void SetSeed(int seed)
			{
				_rand.seed(seed);
			}

			// Iterators.
			iterator begin() { return _moves.begin(); }
  			iterator end() { return _moves.end(); }
	};
}
