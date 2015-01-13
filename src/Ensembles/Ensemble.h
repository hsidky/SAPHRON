#pragma once

#include "../Models/BaseModel.h"
#include "../Moves/Move.h"
#include <vector>

using namespace Models;
using namespace Moves;

namespace Ensembles
{
    // Base class for simulation Ensembles. An Ensemble is provided with a pointer
    // to an instantiated model. The caller must also add Monte Carlo moves to the
    // move queue which holds pointers to various Move objects. The Ensemble template
    // represents both the type of object returned from a call to DrawSample, and the
    // template type of the moves. Each time a sample is drawn, all the moves are
    // performed on the site, after which the probability of acceptance
    // is calculated and the move is either accepted or rejected.
    template <typename T>
    class Ensemble
    {
        private:

            // Number of sweeps performed.
            int _sweeps = 0;

        protected:
          // Pointer to model.
          BaseModel& model;

          // Vector of moves to perform on each site.
          std::vector<Move<T>*> moves;

          // Increment the sweep counter.
          int IncrementSweeps(int num = 1)
          {
              return _sweeps += num;
          }

        public:
            Ensemble (BaseModel& model): model(model) {};

            // Performs one Monte-Carlo iteration. This is defined
            virtual void Iterate() = 0;

            // Performs Monte Carlo moves on a drawn sample and returns pointer
            // to sampled type.

            // Undo previous moves on last drawn sample.
            void UndoMoves()
            {
                for(auto &move : moves)
                  move.Undo();
            }

            // Adds a move to the end of the move queue.
            void AddMove(Move<T>* move)
            {
                return moves.push_back(move);
            }

            // Removes a move from the end of the move queue.
            void RemoveMove()
            {
                moves.pop_back();
            }

            // Get number of sweeps per
            int GetSweepCount()
            {
              return _sweeps;
            }

            // Resets the sweep counter.
            void ResetSweepCount()
            {
                _sweeps = 0;
            }

            // Defines the acceptance probability based on a difference in energy.
            virtual double AcceptanceProbability(double prevH, double currH) = 0;
    };
}
