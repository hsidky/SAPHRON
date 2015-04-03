#include "NVTEnsemble.h"

namespace SAPHRON
{
	inline void NVTEnsemble::Iterate()
	{
		for (int i = 0; i < _world.GetParticleCount(); ++i)
		{
			// Draw sample, evaluate energy.
			_world.DrawRandomParticles(_particles);
			double prevH = _ffmanager.EvaluateHamiltonian(_particles);

			// Select a random move and perform.
			auto move = _mmanager.SelectRandomMove();
			move->Perform(_particles);

			// Evaluate energy and accept/reject.
			double currH = _ffmanager.EvaluateHamiltonian(_particles);

			if(AcceptanceProbability(prevH, currH) < _rand.doub())
				move->Undo();
			else
				_energy += (currH - prevH);
		}

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