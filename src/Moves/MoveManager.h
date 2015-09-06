#pragma once

#include "../Rand.h"
#include "Move.h"
#include "../Observers/Visitable.h"
#include <numeric>
#include <algorithm>

namespace SAPHRON
{
	// Class for managing moves. Moves are added with integer relative 
	// probabilities which are converted into a probability distribution 
	// between [0, 1]. 
	class MoveManager : public Visitable
	{
	private:
		typedef std::vector<Move*> MoveList;
		std::vector<int> _prob;
		std::vector<double> _normprob;
		Rand _rand;
		MoveList _moves;
		int _seed;

		void NormalizeProbabilities()
		{
			// Re-normalize probabilities.
			double sum = std::accumulate(_prob.begin(), _prob.end(), 0);
			_normprob.resize(_prob.size());

			for(size_t i = 0; i < _normprob.size(); ++i)
				_normprob[i] = _prob[i]/sum;
			
			for(size_t i = 1; i < _normprob.size(); ++i)
				_normprob[i] = _normprob[i-1] + _normprob[i];

			// Verify new sum.
			sum = std::accumulate(_normprob.begin(), _normprob.end(), 0);
			if(std::abs(sum - 1.0) > 1e-9)
			{
				std::cerr << "Unkown error occurred normalizing move probabilities." << std::endl;
				exit(-1);
			}
		}

	public:
		typedef MoveList::iterator iterator;
		typedef MoveList::const_iterator const_iterator;

		MoveManager(int seed = 7654) : 
		_prob(0), _normprob(0), _rand(seed), 
		_moves(0), _seed(seed){}

		// Add a move to the move queue with a relative probability (default 1).
		void AddMove(Move* move, unsigned int probability = 1)
		{
			_moves.push_back(move);
			_prob.push_back(probability);

			NormalizeProbabilities();
		}

		// Removes a move.
		void RemoveMove(Move* move)
		{
			auto it = std::find(_moves.begin(), _moves.end(), move);
			if(it != _moves.end())
			{
				size_t pos = it - _moves.begin();
				_prob.erase(pos + _prob.begin());
				_normprob.erase(pos + _normprob.begin());
				_moves.erase(it);
				NormalizeProbabilities();
			}
		}

		// Select a move by index.
		Move* SelectMove(size_t i)
		{
			assert(i < _moves.size());
			return _moves[i];
		}

		// Get the number of moves.
		int GetMoveCount() const { return (int)_moves.size(); }

		// Reset acceptance ratio on moves.
		void ResetMoveAcceptances()
		{
			for(auto& move : _moves)
				move->ResetAcceptanceRatio();
		}

		// Select a random move.
		inline Move* SelectRandomMove()
		{
			auto pos = std::lower_bound(_normprob.begin(), _normprob.end(), _rand.doub()) - _normprob.begin();
			return _moves[pos];
		}

		// Set seed.
		void SetSeed(int seed)
		{
			_seed = seed;
			_rand.seed(seed);
		}

		// Get seed.
		int GetSeed() const	{ return _seed; }

		// Accept a visitor.
		virtual void AcceptVisitor(class Visitor &v)
		{
			v.Visit(*this);
		}

		// Iterators.
		iterator begin() { return _moves.begin(); }
		iterator end() { return _moves.end(); }
	};
}
