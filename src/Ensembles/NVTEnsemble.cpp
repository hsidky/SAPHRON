#include "NVTEnsemble.h"

namespace SAPHRON
{
	inline void NVTEnsemble::Iterate()
	{
		_mmanager->ResetMoveAcceptances();
		
		// Select random move and perform.
		auto move = _mmanager->SelectRandomMove();
		move->Perform(_wmanager, _ffmanager, MoveOverride::None);			
		
		UpdateAcceptances();
		this->IncrementIterations();
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	}

	// Run the NVT ensemble for a specified number of iterations.
	void NVTEnsemble::Run(int iterations)
	{
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}