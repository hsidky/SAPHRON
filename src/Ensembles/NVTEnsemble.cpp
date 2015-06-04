#include "NVTEnsemble.h"

namespace SAPHRON
{
	inline void NVTEnsemble::Iterate()
	{
		_mmanager.ResetMoveAcceptances();
		for (int i = 0; i < _world.GetParticleCount(); ++i)
		{

			// Select random move.
			auto move = _mmanager.SelectRandomMove();

			// Draw sample, evaluate energy.
			move->Draw(_world, _particles);
			auto prevH = _ffmanager.EvaluateHamiltonian(_particles);

			// Perform move.
			EPTuple currH;
			if(move->Perform(_world, _particles))
				currH = _ffmanager.EvaluateHamiltonian(_world);
			else
				currH = _ffmanager.EvaluateHamiltonian(_particles);

			if(AcceptanceProbability(prevH.energy.total(), currH.energy.total()) < _rand.doub())
				move->Undo();
			else
				_eptuple += (currH - prevH);
		}

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