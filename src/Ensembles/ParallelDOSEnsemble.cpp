#include "SemiGrandDOSEnsemble.h"
#include "ParallelDOSEnsemble.h"
#include "WangLandauDOSEnsemble.h"
#include <armadillo>

namespace Ensembles
{
	template<typename T, typename DOSEnsemble>
	void ParallelDOSEnsemble<T,DOSEnsemble>::ConfigureWalkers(double minP,
	                                                          double maxP,
	                                                          int walkers,
	                                                          double overlap)
	{
		if(overlap < 0 || overlap > 1)
		{
			std::cerr
			<< "Invalid overlap. It must be between 0 and 1."
			<< std::endl;
			exit(-1);
		}

		if(walkers < 1)
		{
			std::cerr <<
			"The number of windows must be equal to or greater than 1."
			          << std::endl;
			exit(-1);
		}

		int procs = std::thread::hardware_concurrency();
		if(walkers > procs)
			std::cout <<
			"WARNING: There are " << walkers << " walkers specified but only "
			          << procs <<
			" processors. This will severly impact performance." <<
			std::endl;

		// Initialize threadpool.
		_pool = std::unique_ptr<ThreadPool>(new ThreadPool(walkers));

		using namespace arma;

		// Solve for the appropriate windows.
		int n = 2*walkers;
		mat A(n, n, fill::zeros);
		vec b(n);

		A(0,0) = 1;
		A(n-1,n-1) = 1;
		b(0) = minP;
		b(n-1) = maxP;

		for(int i = 1; i < n-1; i++)
		{
			A(i,i-1) = (i % 2) ? overlap : -1;
			A(i,i) = 1 - overlap;
			A(i,i+1) = (i % 2) ? -1 : overlap;
		}

		vec x = solve(A,b);

		for(int i = 0; i < n-1; i += 2)
			_intervals.emplace_back(x(i), x(i+1));
	}

	template<typename T, typename DOSEnsemble>
	void ParallelDOSEnsemble<T,DOSEnsemble>::Run(int iterations)
	{
		std::vector<std::future<void> > futures;
		for(int i = 0; i < _walkers; i++)
			futures.emplace_back(
			        _pool->enqueue(
			                [this, i,
			                 iterations] { _objects[i]->Run(iterations);
					}
			                )
			        );

		for (auto& it : futures)
			it.get();
	}

	template<typename T, typename DOSEnsemble>
	void ParallelDOSEnsemble<T,DOSEnsemble>::Sweep()
	{
		std::vector<std::future<void> > futures;
		for(int i = 0; i < _walkers; i++)
			futures.emplace_back(
			        _pool->enqueue(
			                [this, i] { _objects[i]->Sweep();
					}
			                )
			        );

		for (auto& it : futures)
			it.get();
	}

	template<typename T, typename DOSEnsemble>
	void ParallelDOSEnsemble<T,DOSEnsemble>::Iterate()
	{
		std::vector<std::future<void> > futures;
		for(int i = 0; i < _walkers; i++)
			futures.emplace_back(
			        _pool->enqueue(
			                [this, i] { _objects[i]->Iterate();
					}
			                )
			        );

		for (auto& it : futures)
			it.get();
	}

	template class ParallelDOSEnsemble<Site, WangLandauDOSEnsemble<Site> >;
	template class ParallelDOSEnsemble<Site, SemiGrandDOSEnsemble<Site> >;
}
