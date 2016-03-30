#pragma once 

#include "Move.h"
#include "../Utils/Rand.h"
#include "../Observers/Visitable.h"
#include "../JSON/Serializable.h"

namespace SAPHRON
{
	// Class for managing moves. Moves are added with integer relative 
	// probabilities which are converted into a probability distribution 
	// between [0, 1]. 
	class MoveManager : public Visitable, public Serializable
	{
	private:
		typedef std::vector<Move*> MoveList;
		std::vector<double> _prob;
		std::vector<double> _normprob;
		Rand _rand;
		MoveList _moves;
		uint _seed;

		void NormalizeProbabilities()
		{
			// Re-normalize probabilities.
			auto sum = std::accumulate(_prob.begin(), _prob.end(), 0.);
			_normprob.resize(_prob.size());

			for(size_t i = 0; i < _normprob.size(); ++i)
				_normprob[i] = _prob[i]/sum;

			for(size_t i = 1; i < _normprob.size(); ++i)
				_normprob[i] = _normprob[i-1] + _normprob[i];
		}

	public:
		typedef MoveList::iterator iterator;
		typedef MoveList::const_iterator const_iterator;

		MoveManager(uint seed = 7654) : 
		_prob(0), _normprob(0), _rand(seed), 
		_moves(0), _seed(seed){}

		// Add a move to the move queue with a relative probability (default 1).
		void AddMove(Move* move, double probability = 1.)
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
				auto pos = it - _moves.begin();
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
		uint GetMoveCount() const { return _moves.size(); }

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
		void SetSeed(unsigned seed)
		{
			_seed = seed;
			_rand.seed(seed);
		}

		// Get seed.
		uint GetSeed() const { return _seed; }

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			auto& moves = json["moves"];
			for(int i = 0; i < (int)_moves.size(); ++i)
			{
				//_moves[i]->Serialize(moves[i]);
				moves[i]["weight"] = _prob[i];
			}
		}

		// Accept a visitor.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
		}

		// Iterators.
		iterator begin() { return _moves.begin(); }
		iterator end() { return _moves.end(); }
	};
}