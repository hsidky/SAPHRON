#include "DOSEnsemble.h"

namespace SAPHRON
{
	// Run until the histogram has been flattened at the set scale factor.
	inline void DOSEnsemble::Iterate()
	{
		_flatness = _hist.CalculateFlatness();
		while(_flatness < GetTargetFlatness())
		{
			_acceptedCount = 0;
			for (int i = 0; i < _world.GetParticleCount(); ++i)
			{
			
				// Select random move.
				auto move = _mmanager.SelectRandomMove();
	
				// Draw sample, evaluate energy.
				_world.DrawRandomParticles(_particles, move->RequiredParticles());
				double prevH = _ffmanager.EvaluateHamiltonian(_particles);
				double prevO = _orderp.EvaluateParameter(_energy);

				// Perform move.
				move->Perform(_particles);
	
				// Evaluate new energy and accept/reject.
				double currH = _ffmanager.EvaluateHamiltonian(_particles);
				double newE = _energy + (currH - prevH);
				double newO = _orderp.EvaluateParameter(newE);

				if(AcceptanceProbability(_energy, prevO, newE, newO) < _rand.doub())
				{
					move->Undo();
					newO = prevO;
				}
				else
				{
					_energy = newE;
					++_acceptedCount;
				}
				
				// Update bins and energy (log DOS).
				int bin = _hist.Record(newO);
				_hist.UpdateValue(bin, _hist.GetValue(bin) + _sf);	
			}

			_flatness = _hist.CalculateFlatness();
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
			ResetHistogram();
			ReduceScaleFactor();
		}
		this->NotifyObservers(SimEvent(this, this->GetIteration(), true));
	}
}