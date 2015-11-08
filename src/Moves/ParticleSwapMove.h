#pragma once 

#include "Move.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"

namespace SAPHRON
{
	// Class for performing a particle swap between two worlds. It removes a particle from 
	// a random world and inserts it randomly into another in a random location.
	class ParticleSwapMove : public Move
	{
	private:
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:
		ParticleSwapMove(int seed = 3429329) : 
		_rand(seed), _rejected(0), _performed(0), _seed(seed)
		{
		}


		// Move particle from world 1 to world 2.
		void MoveParticle(Particle* particle, World* w1, World* w2)
		{
			// Remove particle from its world. 
			// Neighbor list is cleared automatically.
			w1->RemoveParticle(particle);

			// Generate random position for particle insertion.
			const auto& H = w2->GetHMatrix();
			Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
			particle->SetPosition(H*pr);

			// Neighbor list is updated automatically.
			w2->AddParticle(particle);
			++_performed;
		}

		// Swap a random particle from a random world to another random world.
		virtual void Perform(WorldManager* wm, 
							 ForceFieldManager* ffm, 
							 const MoveOverride& override) override
		{
			if(wm->GetWorldCount() < 2)
			{
				std::cerr << 
				"Cannot perform particle swap move on less than 2 worlds." << 
				std::endl;
				exit(-1);
			}

			// Pick two random worlds for particle swap.
			World* w1 = wm->GetRandomWorld();
			World* w2 = wm->GetRandomWorld();

			// If world is empty, return. 
			if(w1->GetParticleCount() == 0)
				return;

			// Make sure we pick a different world.
			while(w2 == w1)
				w2 = wm->GetRandomWorld();

			// Get volumes.
			double v1 = w1->GetVolume();
			double v2 = w2->GetVolume();

			// Get random particle, eval E.
			Particle* particle = w1->DrawRandomParticle();
			Position pi = particle->GetPosition();
			auto ei = ffm->EvaluateHamiltonian(*particle, w1->GetComposition(), v1);

			// Move particle from w1 to w2.
			MoveParticle(particle, w1, w2);
			auto& comp1 = w1->GetComposition();
			auto& comp2 = w2->GetComposition();
			auto id = particle->GetSpeciesID();
			double n1 = comp1[id];
			double n2 = comp2[id];

			// Evaluate new energy and accept/reject.
			auto ef = ffm->EvaluateHamiltonian(*particle, w2->GetComposition(), v2);
			double de = ef.energy.total() - ei.energy.total();
			
			// The acceptance rule is from Frenkel & Smit Eq. 8.3.4.
			// However, it was modified for *final* particle numbers.
			auto& sim = SimInfo::Instance();
			double beta = 1.0/(sim.GetkB()*w2->GetTemperature());
			double p = (n1 - 1.0)*v2/(n2*v1)*exp(-beta*de);
			p = p > 1.0 ? 1.0 : p;

			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				w2->RemoveParticle(particle);
				particle->SetPosition(pi);
				w1->AddParticle(particle);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w1->IncrementEnergy(-1.0*ei.energy);
				w1->IncrementPressure(-1.0*ei.pressure);
				w2->IncrementEnergy(ef.energy);
				w2->IncrementPressure(ef.pressure);
			}
		}

		virtual void Perform(World*, 
							 ForceFieldManager*, 
							 DOSOrderParameter* , 
							 const MoveOverride&) override
		{
			std::cerr << "Particle swap move does not support DOS ensemble." << std::endl;
			exit(-1);
		}

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "ParticleSwap";
			json["seed"] = _seed;
		}

		virtual std::string GetName() const override { return "ParticleSwap"; }

		// Clone move.
		Move* Clone() const override
		{
			return new ParticleSwapMove(static_cast<const ParticleSwapMove&>(*this));
		}

	};
}