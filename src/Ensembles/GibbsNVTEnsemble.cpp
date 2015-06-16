#include "GibbsNVTEnsemble.h"

namespace SAPHRON
{
	inline void GibbsNVTEnsemble::Iterate()
	{
		_mmanager.ResetMoveAcceptances();
		_psmove->ResetAcceptanceRatio();
		_vsmove->ResetAcceptanceRatio();
		for (unsigned int i = 0; i < _ntotal; ++i)
		{
			double rnd = _rand.doub();

			// Volume scaling move.
			if(rnd < 0.005)
			{
				_vsmove->Draw(_worlds, _windex, _particles);
				
				int w1 = _windex[0];
				int w2 = _windex[1];

				double prevV1 = _worlds[w1]->GetVolume();
				double prevV2 = _worlds[w2]->GetVolume();

				_vsmove->Perform(_worlds, _windex, _particles);
				
				auto currH1 = _ffmanagers[w1]->EvaluateHamiltonian(*_worlds[w1]);
				auto currH2 = _ffmanagers[w2]->EvaluateHamiltonian(*_worlds[w2]);

				if(VolumeScaleAcceptance(_eptuple[w1].energy.total(), prevV1, 
										 _eptuple[w2].energy.total(), prevV2, 
										 currH1.energy.total(), _worlds[w1]->GetVolume(), 
										 currH2.energy.total(), _worlds[w2]->GetVolume(),
										 _worlds[w1]->GetParticleCount(), _worlds[w2]->GetParticleCount())
					< _rand.doub())
					_vsmove->Undo();		
				else
				{
					_eptuple[w1] = currH1;
					_eptuple[w2] = currH2; 
				}
			} 
			// Particle swap move.
			else if(rnd < 0.015)
			{
				_psmove->Draw(_worlds, _windex, _particles);
				if(_windex.size() == 0)
					continue;
				int w1 = _windex[0];
				int w2 = _windex[1];

				double V1 =_worlds[w1]->GetVolume();
				double V2 =_worlds[w2]->GetVolume();
				
				auto prevH = _ffmanagers[w1]->EvaluateHamiltonian(_particles, _worlds[w1]->GetComposition(), V1);
				_psmove->Perform(_worlds, _windex, _particles);
				auto currH = _ffmanagers[w2]->EvaluateHamiltonian(_particles, _worlds[w2]->GetComposition(), V2);
				
				int N1 = _worlds[w1]->GetParticleCount();
				int N2 = _worlds[w2]->GetParticleCount();

				if(ParticleSwapAcceptance(prevH.energy.total(), currH.energy.total(), 
					V1, V2, N1, N2) < _rand.doub())
					_psmove->Undo();
				else
				{
					_eptuple[w1] -= prevH;
					_eptuple[w2] += currH;
				}
			}
			// Move manager.
			else 
			{
				// Select random move.
				auto move = _mmanager.SelectRandomMove();

				// Select random world;
				int w = _rand.int32() % _worlds.size();
				World* world = _worlds[w];

				// Draw sample, evaluate energy.
				move->Draw(*world, _particles);
				auto prevH = _ffmanagers[w]->EvaluateHamiltonian(_particles, world->GetComposition(), world->GetVolume());

				// Perform move.
				EPTuple currH;
				if(move->Perform(*world, _particles))
					currH = _ffmanagers[w]->EvaluateHamiltonian(*world);
				else
					currH = _ffmanagers[w]->EvaluateHamiltonian(_particles, world->GetComposition(), world->GetVolume());

				// Check for neighborlist updates.
				world->CheckNeighborListUpdate(_particles);

				// Acceptance probability.
				if(AcceptanceProbability(prevH.energy.total(), currH.energy.total()) < _rand.doub())
					move->Undo();
				else
					_eptuple[w] += (currH - prevH);
			}
		}

		UpdateAcceptances();
		this->IncrementIterations();
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	}

	void GibbsNVTEnsemble::Run(int iterations)
	{
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}