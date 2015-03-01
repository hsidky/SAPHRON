#pragma once

#include "../../include/ThreadPool.h"
#include "../Models/BaseModel.h"
#include "DensityOfStatesEnsemble.h"
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Ensembles
{
	template <typename T, typename DOSEnsemble>
	class ParallelDOSEnsemble : public DensityOfStatesEnsemble<T>
	{
		private:
			// Vector of ensemble walkers.
			std::vector<std::shared_ptr<DOSEnsemble> > _objects;

			// Vector of models.
			std::vector<std::shared_ptr<BaseModel> > _models;

			// Vector of move copies.
			std::vector<std::shared_ptr<Move<T> > > _cMoves;

			// Vector of interval pairs for each ensemble.
			std::vector<Interval> _intervals;

			// Thread pool for walkers.
			std::unique_ptr<ThreadPool> _pool;

			// Calculates the appropriate intervals for each walker for a given global
			// interval, number of walkers and overlap.
			void ConfigureWalkers(double minP, double maxP, int walkers, double overlap);

			// Number of walkers.
			int _walkers;

		public:
			template<typename ... Args>
			ParallelDOSEnsemble(BaseModel& model,
			                    double minP,
			                    double maxP,
			                    int binCount,
			                    int walkers,
			                    double overlap,
			                    Args && ... args)
				: DensityOfStatesEnsemble<T>(model, minP, maxP, binCount),
				  _walkers(walkers)
			{
				ConfigureWalkers(minP, maxP, walkers, overlap);

				// Duplicate model.
				for(int i = 0; i < walkers; i++ )
					_models.emplace_back(model.Clone());

				for(int i = 0; i < walkers; i++)
				{
					_objects.emplace_back(new DOSEnsemble(*_models[i].get(),
					                                      _intervals[i].first,
					                                      _intervals[i].second,
					                                      binCount,
					                                      args ...));
					_objects.back()->SetWalkerID(i);
				}

				// Update intervals based on DOS ensemble.
				for(int i = 0; i < walkers; i++)
					_intervals[i] = _objects[i]->GetParameterInterval();
			};

			void Run(int iterations) override;

			void Sweep() override;

			void Iterate() override;

			double AcceptanceProbability(double, double) override
			{
				std::cerr << "ParalellDOSEnsemble does not have AcceptanceProbability defined." <<
				std::endl;
				exit(-1);
			}

			// Adds a move to the end of the move queue for all parallel ensembles.
			void AddMove(Move<T>& move) override
			{
				for(int i = 0; i < _walkers; i++)
				{
					_cMoves.emplace_back(move.Clone());
					_objects[i]->AddMove(*_cMoves.back().get());
				}

				DensityOfStatesEnsemble<T>::AddMove(move);
			}

			double SetScaleFactor(double sf) override
			{
				for(int i = 0; i < _walkers; i++)
					_objects[i]->SetScaleFactor(sf);

				return DensityOfStatesEnsemble<T>::SetScaleFactor(sf);
			}

			double SetTargetFlatness(double f) override
			{
				for(int i = 0; i < _walkers; i++)
					_objects[i]->SetTargetFlatness(f);

				return DensityOfStatesEnsemble<T>::SetTargetFlatness(f);
			}

			// Gets the interval for a specified walker. Index from 1 to n.
			Interval GetWalkerInterval(int n)
			{
				assert(n > 0 && (unsigned int) n <= _intervals.size());
				return _intervals[n-1];
			}

			// Gets a shared pointer to an ensemble. Index from 1 to n.
			std::shared_ptr<DOSEnsemble> GetEnsemble(int n)
			{
				assert(n > 0 && (unsigned int) n <= _intervals.size());
				return _objects[n-1];
			}

			// Gets a shared pointer to an ensemble. Index from 1 to n.
			std::shared_ptr<BaseModel> GetModel(int n)
			{
				assert(n > 0 && (unsigned int) n <= _intervals.size());
				return _models[n-1];
			}

			void AcceptVisitor(class Visitor&) override
			{
				std::cerr << "ParalellDOSEnsemble cannot accept visitors" << std::endl;
				exit(-1);
			}

			void AddObserver(SimObserver* observer)
			{
				for(int i = 0; i < _walkers; i++)
					_objects[i]->AddObserver(observer);
			}

			void RemoveObserver(SimObserver* observer)
			{
				for(int i = 0; i < _walkers; i++)
					_objects[i]->RemoveObserver(observer);
			}
	};
}
