#pragma once 

#include "Move.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"
#include "../Worlds/WorldManager.h"
#include "../Utils/Rand.h"

namespace SAPHRON
{
	// Class to scale the volume of a randomly chosen 
	// world, often used in NPT simulations. 
	// The scaling is performed w.r.t. log(V) for a 
	// specified external pressure.
	// Follows Algorithm 11 in Frenkel & Smit.
	class VolumeScaleMove : public Move
	{
	private:
		double _pextern; 
		double _dvmax;
		Rand _rand;
		int _rejected;
		int _performed;
		bool _prefac;
		int _seed;

	public:
		VolumeScaleMove(double Pextern, double dvmax, int seed = 593857) : 
		_pextern(Pextern), _dvmax(dvmax), _rand(seed), _rejected(0), 
		_performed(0), _prefac(false), _seed(seed)
		{
		}

		// Set new volume on world based on old volume.
		// Returns new volume.
		double Perform(World* w, double vi)
		{
			auto lnvn = log(vi) + (_rand.doub() - 0.5)*_dvmax;
			auto vn = exp(lnvn);			
			w->SetVolume(vn, true);
			++_performed;
			return vn;
		}

		virtual void Perform(WorldManager* wm, 
							 ForceFieldManager* ffm, 
							 const MoveOverride& override) override
		{
			auto* w = wm->GetRandomWorld();

			// Record initial volume, energy and particle count.
			auto vi = w->GetVolume();
			auto ei = w->GetEnergy();
			auto n = w->GetParticleCount();

			// Perform the move.
			auto vf = Perform(w, vi);

			// Compute final energy. 
			auto ef = ffm->EvaluateHamiltonian(*w);
			auto de = ef.energy - ei;

			// Compute acceptance rule.
			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(w->GetTemperature()*sim.GetkB());
			auto pconv = sim.GetPressureConv();
			auto arg = -beta*(de.total() + _pextern/pconv*(vf - vi) - (n + 1.0) * log(vf/vi)/beta);
			auto p = exp(arg);
			p = p > 1.0 ? 1.0 : p;

			// Accept or reject.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				w->SetVolume(vi, true);
				++_rejected;
			}
			else
			{
				w->SetEnergy(ef.energy);
				w->SetPressure(ef.pressure);
			}
		}

		virtual void Perform(World* w, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{
			// Record initial volume, energy and particle count.
			auto vi = w->GetVolume();
			auto ei = w->GetEnergy();
			auto pi = w->GetPressure();
			auto n = w->GetParticleCount();
			auto opi = op->EvaluateOrderParameter(*w);

			// Perform the move.
			auto vf = Perform(w, vi);

			// Compute final energy. We update energies early for DOS. 
			auto ef = ffm->EvaluateHamiltonian(*w);
			w->SetEnergy(ef.energy);
			w->SetPressure(ef.pressure);
			auto opf = op->EvaluateOrderParameter(*w);

			// Compute acceptance rule.
			auto pacc = op->AcceptanceProbability(ei, ef.energy, opi, opf, *w);

			if(_prefac)
			{
				auto& sim = SimInfo::Instance();
				auto beta = 1.0/(w->GetTemperature()*sim.GetkB());
				auto arg = -beta*(_pextern*(vf - vi) - (n + 1.0) * log(vf/vi)/beta);
				pacc *= exp(arg);
			}
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Accept or reject.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				w->SetVolume(vi, true);
				w->SetEnergy(ei);
				w->SetPressure(pi);
				++_rejected;
			}
		}

		// Turns on or off the acceptance rule prefactor for DOS order parameter.
		void SetOrderParameterPrefactor(bool flag) { _prefac = flag; }

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0 - (double)_rejected/_performed;
		}

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "VolumeScale";
			json["dv"] = _dvmax;
			json["p_extern"] = _pextern;
			json["seed"] = _seed;
			json["op_prefactor"] = _prefac;
		}

		virtual std::string GetName() const override { return "VolumeScale"; }

		Move* Clone() const override
		{
			return new VolumeScaleMove(static_cast<const VolumeScaleMove&>(*this));
		}

		~VolumeScaleMove(){}
	};
}