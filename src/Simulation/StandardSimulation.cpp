#include "StandardSimulation.h"

namespace SAPHRON
{
	inline void StandardSimulation::Iterate()
	{
		_mmanager->ResetMoveAcceptances();
		
		// Select random move and perform.
		for(int i = 0; i < GetMovesPerIteration(); ++i)
		{
			auto* move = _mmanager->SelectRandomMove();
			move->Perform(_wmanager, _ffmanager, MoveOverride::None);
		}
		
		UpdateAcceptances();
		this->IncrementIterations();
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	}

	// Run the NVT ensemble for a specified number of iterations.
	void StandardSimulation::Run(int iterations)
	{
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}