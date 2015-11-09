#pragma once

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"

namespace SAPHRON
{
	// Class for particle deletion move.
	class DeleteParticleMove : public Move
	{
	private: 
		Rand _rand;
		int _rejected;
		int _performed;
		std::vector<int> _species;
		int _seed;

	public:
		DeleteParticleMove(const std::vector<int>& species, 
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _species(0), 
		_seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			for(auto& id : species)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_species.push_back(id);
			}
		}

		DeleteParticleMove(const std::vector<std::string>& species, 
						   int seed = 45843) : 
		_rand(seed), _rejected(0), _performed(0), _species(0),
		_seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			for(auto& id : species)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_species.push_back(it - list.begin());
			}
		}

		virtual void Perform(WorldManager* wm, 
							 ForceFieldManager* ffm, 
							 const MoveOverride& override) override
		{
			// Get random world.
			World* w = wm->GetRandomWorld();

			// Get random identity from list. 
			size_t n = _species.size();
			assert(n > 0);

			auto id = _species[_rand.int32() % n];

			// If world doesn't have any particles of type "id", return.
			auto& comp = w->GetComposition();
			if(comp[id] == 0)
				return;

			auto* p = w->DrawRandomParticleBySpecies(id);
			if(p == nullptr) // Safety check.
				return;

			auto V = w->GetVolume();
			auto mu = w->GetChemicalPotential(id);
			auto lambda = w->GetWavelength(id);
			auto N = comp[id];

			// Evaluate old energy then remove particle. 
			auto ei = ffm->EvaluateHamiltonian(*p, comp, V);

			w->RemoveParticle(p);
			++_performed;

			// The acceptance rule is from Frenkel & Smit Eq. 5.6.9.
			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());
			auto pacc = (lambda*lambda*lambda*N)/V*exp(beta*(mu + ei.energy.total()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				// Add it back to the world. 
				w->AddParticle(p);
				++_rejected;
			}
			else
			{
				// Stach the particle. 
				w->StashParticle(p, 1);

				// Update energies and pressures.
				w->IncrementEnergy(-1.0*ei.energy);
				w->IncrementPressure(-1.0*ei.pressure);
			}
		}

		virtual void Perform(World*, 
							 ForceFieldManager*, 
							 DOSOrderParameter* , 
							 const MoveOverride&) override
		{
			std::cerr << "DOS for particle insertion is currently unimplemented." << std::endl;
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
			json["type"] = "DeleteParticle";
			json["seed"] = _seed;

			auto& species = Particle::GetSpeciesList();
			for(auto& s : _species)
				json["species"].append(species[s]);
		}

		virtual std::string GetName() const override { return "DeleteParticle"; }

		// Clone move.
		Move* Clone() const override
		{
			return new DeleteParticleMove(static_cast<const DeleteParticleMove&>(*this));
		}

		~DeleteParticleMove(){}
	};
}