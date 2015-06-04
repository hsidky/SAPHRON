#include "DOSEnsemble.h"

namespace SAPHRON
{

	inline double DOSEnsemble::AcceptanceProbability(double prevE, double prevOP, double newE, double newOP)
	{
		if(_hist.GetBin(newOP) == -1)
		{
			if(prevOP < _hist.GetMinimum() && newOP > prevOP)
				return 1.0;
			else if(prevOP > _hist.GetMaximum() && newOP < prevOP)
				return 1.0;

			return 0;
		}

		return _orderp.AcceptanceProbability(prevE, _hist.GetValue(prevOP), newE, _hist.GetValue(newOP));
	}

	// Run until the histogram has been flattened at the set scale factor.
	inline void DOSEnsemble::Iterate()
	{
		_flatness = _hist.CalculateFlatness();
		while(_flatness < GetTargetFlatness())
		{
			_mmanager.ResetMoveAcceptances();
			for (int i = 0; i < _world.GetParticleCount(); ++i)
			{
				// Select random move.
				auto move = _mmanager.SelectRandomMove();
	
				// Draw sample, evaluate energy.
				move->Draw(_world, _particles);
				auto prevH = _ffmanager.EvaluateHamiltonian(_particles);
				double prevOP = _orderp.EvaluateParameter(_eptuple.energy.total());

				// Perform move and re-evaluate energy.
				EPTuple currH;
				if(move->Perform(_world, _particles))
					currH = _ffmanager.EvaluateHamiltonian(_world);
				else
					currH = _ffmanager.EvaluateHamiltonian(_particles);
	
				//Accept/reject.
				auto newE = _eptuple + (currH - prevH);
				double newOP = _orderp.EvaluateParameter(newE.energy.total());

				if(AcceptanceProbability(_eptuple.energy.total(), prevOP, newE.energy.total(), newOP) < _rand.doub())
				{
					move->Undo();
					newOP = prevOP;
				}
				else
					_eptuple = newE;
				
				// Update bins and energy (log DOS).
				int bin = _hist.Record(newOP);
				_hist.UpdateValue(bin, _hist.GetValue(bin) + _sf);	
			}

			UpdateAcceptances();
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