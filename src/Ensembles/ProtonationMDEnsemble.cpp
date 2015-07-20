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

		if(move->GetName()=="ChargeSwap")
		{
			_munew=0.0;

			if(_particles[0]->GetCharge() == _particles[1]->GetCharge())
				return;

		}

		auto prevH = _ffmanager.EvaluateHamiltonian(_particles, _world.GetComposition(), _world.GetVolume());

		// Perform move.
		EPTuple currH;

		move->Perform(_world, _particles);

		if(move->GetName()=="Protonation")
		{
			if(_particles[0]->GetCharge())
				_munew=_mu*(-1.0);
			else
				_munew=_mu;
		}
		
		currH = _ffmanager.EvaluateHamiltonian(_particles, _world.GetComposition(), _world.GetVolume());

		// Acceptance probability charging

		if(AcceptanceProbability(prevH.energy.total(), currH.energy.total()) < _rand.doub())
			move->Undo();
		else
     		_eptuple += (currH - prevH);
		
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