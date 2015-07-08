#include "ProtonationMDEnsemble.h"

namespace SAPHRON
{
	// Current method is hacked until the next update. Constant chemical potential that represents the pH
	// should be accounted for in the AcceptanceProbability move. However, the sign of the chemical potential
	// energy is dependent on the move performed. Thus, the change in energy is reliant on the move performed
	// and these two currently can not be linked
	// TODO: Update move when the Simulation object is coded and created.

	inline void ProtonationMDEnsemble::Iterate()
	{
		_mmanager.ResetMoveAcceptances();

		//Run MC then run MD

		// Select protonation move from manager.
		auto move = _mmanager.SelectRandomMove();

		// Draw sample, evaluate energy.
		move->Draw(_world, _particles);
		auto prevH = _ffmanager.EvaluateHamiltonian(_particles, _world.GetComposition(), _world.GetVolume());

		// Perform move.
		EPTuple currH;

		//If charged vs uncharged
		auto _charged = move->Perform(_world, _particles);

		currH = _ffmanager.EvaluateHamiltonian(_particles, _world.GetComposition(), _world.GetVolume());

		// Acceptance probability charging
		if(_charged)
		{
			if(AcceptanceProbability(prevH.energy.total(), currH.energy.total()+_mu) < _rand.doub())
				move->Undo();
			else
				_eptuple += (currH - prevH);

		}
		
		// Acceptance probability uncharging
		else
		{
			if(AcceptanceProbability(prevH.energy.total(), currH.energy.total()-_mu) < _rand.doub())
				move->Undo();
			else
				_eptuple += (currH - prevH);

		}
		
		UpdateAcceptances();
		this->IncrementIterations();
		this->NotifyObservers(SimEvent(this, this->GetIteration()));

		//Run MD here


	}

	// Run the Protonation ensemble for a specified number of iterations.
	void ProtonationMDEnsemble::Run(int iterations)
	{
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}