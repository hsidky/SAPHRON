#pragma once

#include "../Rand.h"
#include "Move.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Simulation/SimInfo.h"

namespace SAPHRON
{
	// Random identity move. Assigns a random identity (species) to a primitive 
	// (atom/spin) type from a list of identities. 
	class RandomIdentityMove : public Move
	{
	private:
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;
		std::vector<int> _identities;

	public:
		// Initialize identity change move from a list of species IDs.
		RandomIdentityMove(const std::vector<int>& identities, int seed = 1337) :
			_rand(seed), _rejected(0), _performed(0), _seed(seed), _identities(0)
		{
			// Verify the identities exist.
			auto list = Particle::GetSpeciesList();
			for(auto& id : identities)
			{
				if(id >= list.size())
				{
					std::cerr << "Specied ID provided does not exist." << std::endl;
					exit(-1);
				}
				_identities.push_back(id);
			}
		}

		// Initialize identity change move from a list of species names.
		RandomIdentityMove(const std::vector<std::string>& identities, int seed = 1337) :
			_rand(seed), _rejected(0), _performed(0), _seed(seed), _identities(0)
		{
			// Verify the identities exist.
			auto list = Particle::GetSpeciesList();
			for(auto& id : identities)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Specied ID provided does not exist." << std::endl;
					exit(-1);
				}
				_identities.push_back(it - list.begin());
			}
		}

		// Reassigns the species of a site with a new random one.
		void Perform(Particle* particle)
		{
			particle->SetSpecies(_identities[_rand.int32() % _identities.size()]);
			++_performed;
		}

		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			// Get random particle from random world.
			World* w = wm->GetRandomWorld();
			Particle* particle = w->DrawRandomParticle();
			
			// Get initial species and evaluate energy.
			auto si = particle->GetSpeciesID();
			auto ei = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());

			// Perform move and evaluate new energy.
			Perform(particle);
			auto ef = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());
			Energy de = ef.energy - ei.energy;

			// Get sim info for kB.
			auto sim = SimInfo::Instance();

			// Acceptance probability.
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				particle->SetSpecies(si);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->SetEnergy(w->GetEnergy() + de);
				w->SetPressure(w->GetPressure() + (ef.pressure - ei.pressure));
			}	
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

		// Get seed.
		virtual int GetSeed() const override { return _seed; }

		virtual std::string GetName() const override { return "RandomIdentity"; }

		// Clone move.
		virtual Move* Clone() const override
		{
			return new RandomIdentityMove(
			               static_cast<const RandomIdentityMove&>(*this)
			               );
		}
	};
}
