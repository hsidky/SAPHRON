#include "DOSEnsemble.h"

namespace SAPHRON
{
	void DOSEnsemble::Iterate()
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
				int bin = _hist->Record(_orderp->EvaluateOrderParameter(*world));
				_hist->UpdateValue(bin, _hist->GetValue(bin) + _f);
			}

			// Reset histogram if desired.
			if(_hreset && this->GetIteration() % _hreset == 0)
				_hist->ResetHistogram();
			
			_flatness = _hist->CalculateFlatness();
			UpdateAcceptances();
			this->IncrementIterations();
			this->NotifyObservers(SimEvent(this, this->GetIteration()));
		}
	}

	// Run the DOS algorithm for a specified number of scale factor reductions.
	void DOSEnsemble::Run(int iterations)
	{
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
		for(int i = 0; i < iterations; ++i)
		{
			Iterate();
			_hist->ResetHistogram();
			ReduceConvergenceFactor();
		}
		this->NotifyObservers(SimEvent(this, this->GetIteration(), true));
	}
}