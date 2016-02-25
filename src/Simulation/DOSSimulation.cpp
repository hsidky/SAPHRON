#include "DOSSimulation.h"

namespace SAPHRON
{
	void DOSSimulation::Iterate()
	{
		_flatness = _hist->CalculateFlatness();
		while(_flatness < GetTargetFlatness())
		{
			_mmanager->ResetMoveAcceptances();
			for(int i = 0; i < GetMovesPerIteration(); ++i)
			{
				// Get world 0. Multiple worlds not supported. 
				World* world = _wmanager->GetWorld(0);
				auto* move = _mmanager->SelectRandomMove();

				// Perform move. 
				move->Perform(world, _ffmanager, _orderp, MoveOverride::None);

				// Update bins and histogram. 
				_opval = _orderp->EvaluateOrderParameter(*world);
				int bin = _hist->Record(_opval);
				_hist->UpdateValue(bin, _hist->GetValue(bin) + _f);
			}

			// Reset histogram if desired.
			if(this->GetIteration() && _hreset && (this->GetIteration() % _hreset == 0))
				_hist->ResetHistogram();
			
			_flatness = _hist->CalculateFlatness();
			UpdateAcceptances();
			this->IncrementIterations();

			#ifdef MULTI_WALKER
			// Sync periodically.
			if(this->GetIteration() % _syncfreq == 0)
			{
				// Combine histograms.
				_hist->ReduceValues();
				boost::mpi::broadcast(_comm, _f, 0);
			}

			if(_comm.rank() == 0)
			#endif
			this->NotifyObservers(SimEvent(this, this->GetIteration()));
		}
	}

	// Run the DOS algorithm for a specified number of scale factor reductions.
	void DOSSimulation::Run(int iterations)
	{
		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration()));

		// Cheap hack. Slaves iterate forever.
		#ifdef MULTI_WALKER
		if(_comm.rank() != 0)
			iterations = 1e5;
		#endif

		for(int i = 0; i < iterations; ++i)
		{
			Iterate();
			_hist->ResetHistogram();

			// Only root walker updates convergence factor.
			#ifdef MULTI_WALKER
			if(_comm.rank() == 0)
			{
			#endif
			ReduceConvergenceFactor();			
			#ifdef MULTI_WALKER
			}
			#endif
		}

		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration(), true));
	}
}