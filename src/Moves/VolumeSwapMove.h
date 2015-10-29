#pragma once 

#include "Move.h"
#include "../Simulation/SimInfo.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Utils/Rand.h"

namespace SAPHRON
{
	// Class to swap volume between two randomly chosen worlds (boxes). 
	// This is often used for Gibbs ensemble simulations. 
	// The total volume between two boxes is fixed but isswapped between them. 
	// Note that the scaling is performed w.r.t. log(V). 
	// Currently only does cubic volume scales. 
	// Follows Algorithm 18 in Frenkel & Smit.
	class VolumeSwapMove : public Move
	{
	private:
		double _dvmax;
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:
		VolumeSwapMove(double dvmax, int seed = 64866) : 
		_dvmax(dvmax), _rand(seed),
		_rejected(0), _performed(0), _seed(seed)
		{
		}

		// Swap some volume <= dvmax between w1 and w2 where v1 and 
		// v2 are the respective initial volumes.
		void Perform(World* w1, World* w2, double v1, double v2)
		{
			double v = v1 + v2;
			double lnvn = log(v1/v2) + (_rand.doub() - 0.5)*_dvmax;
			double v1n = v*exp(lnvn)/(1+exp(lnvn));
			double v2n = v - v1n;
			
			double b1 = pow(v1n, 1.0/3.0);
			double b2 = pow(v2n, 1.0/3.0);
			w1->SetBoxVectors(b1, b1, b1, true);
			w2->SetBoxVectors(b2, b2, b2, true);
			++_performed;
		}

		// Explicit volume swap move (helper function). 
		void Perform(World* w1, World* w2)
		{
			double v1 = w1->GetVolume();
			double v2 = w2->GetVolume();
			Perform(w1, w2, v1, v2);
		}

		// Perform volume swap move.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			if(wm->GetWorldCount() < 2)
			{
				std::cerr << "Cannot perform volume swap move on less than 2 worlds." << std::endl;
				exit(-1);
			}

			// Select two random worlds.
			World* w1 = wm->GetRandomWorld();
			World* w2 = wm->GetRandomWorld();

			// Make sure we pick a different world.
			while(w2 == w1)
				w2 = wm->GetRandomWorld();

			// Record initial volumes.
			double vi1 = w1->GetVolume();
			double vi2 = w2->GetVolume();

			// Record initial energies.
			Energy ei1 = w1->GetEnergy();
			Energy ei2 = w2->GetEnergy();

			// Get particle numbers. 
			int n1 = w1->GetParticleCount();
			int n2 = w2->GetParticleCount();

			// Perform volume swap.
			Perform(w1, w2, vi1, vi2);

			// Get final volumes.
			double vf1 = w1->GetVolume();
			double vf2 = w2->GetVolume();

			// Get final energies.
			EPTuple ef1 = ffm->EvaluateHamiltonian(*w1);
			EPTuple ef2 = ffm->EvaluateHamiltonian(*w2);

			// Calculate delta E.
			double de1 = ef1.energy.total() - ei1.total();
			double de2 = ef2.energy.total() - ei2.total();

			// Get SimInfo for kB.
			auto sim = SimInfo::Instance();

			// Calculate acceptance.
			double beta = 1.0/(sim.GetkB()*w1->GetTemperature());
			double a1 = -beta*(de1 - (n1 + 1)*log(vf1/vi1)/beta);
			double a2 = -beta*(de2 - (n2 + 1)*log(vf2/vi2)/beta);
			double p = exp(a1 + a2);
			p = p > 1.0 ? 1.0 : p;
			
			// Undo move if it doesn't meet probability.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				double b1 = pow(vi1, 1.0/3.0);
				double b2 = pow(vi2, 1.0/3.0);
				w1->SetBoxVectors(b1, b1, b1, true);
				w2->SetBoxVectors(b2, b2, b2, true);
				++_rejected;
			}
			else
			{
				// Update energies / pressures. 
				w1->SetEnergy(ef1.energy);
				w1->SetPressure(ef1.pressure);
				w2->SetEnergy(ef2.energy);
				w2->SetPressure(ef2.pressure);
			}			
		}

		// Perform move using DOS interface (not!).
		virtual void Perform(World*, ForceFieldManager*, DOSOrderParameter*, const MoveOverride&) override
		{
			std::cerr << "Volume swap move does not support DOS interface." << std::endl;
			exit(-1);
		}

		// Get maximum volume change.
		double GetMaxVolumeChange() const { return _dvmax; }
	
		// Get move acceptance ratio.	
		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		// Reset move acceptance ratio.
		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Serialize.
		virtual void Serialize(Json::Value& root) const override
		{
			Json::Value val;
			val["type"] = "VolumeSwap";
			val["seed"] = _seed;
			val["dv"] = _dvmax;
			root["moves"].append(val);
		}

		// Get seed.
		virtual int GetSeed() const override { return _seed; }

		// Get move name.
		virtual std::string GetName() const override { return "VolumeSwap"; }

		// Clone move.
		Move* Clone() const override
		{
			return new VolumeSwapMove(static_cast<const VolumeSwapMove&>(*this));
		}
	};
}