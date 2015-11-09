#pragma once

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"

namespace SAPHRON
{
	// Class for particle insertion move. Based on providing 
	// a list of species, the move will stash "stashcount" copies 
	// of each one, in each world in the world manager. However, 
	// if the number of stashed particles runs out (say, due to deletion)
	// then they are automatically replenished by the world, at an expense.
	class InsertParticleMove : public Move
	{
	private: 
		Rand _rand;
		int _rejected;
		int _performed;
		std::vector<int> _species;
		int _scount; // Stash count.
		int _seed;

		void InitStashParticles(const WorldManager& wm)
		{
			// Get particle map, find one of the appropriate species 
			// and clone.
			auto& plist = Particle::GetParticleMap();
			for(auto& id : _species)
			{
				// Since the species exists, we assume there must be at least 
				// one find. 
				auto pcand = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);

				// Stash a characteristic amount of the particles in world.
				for(auto& world : wm)
					world->StashParticle(pcand->second, _scount);
			}
		}

	public:
		InsertParticleMove(const std::vector<int>& species, 
						   const WorldManager& wm,
						   int stashcount,
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _species(0), 
		_scount(stashcount), _seed(seed)
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

			InitStashParticles(wm);
		}

		InsertParticleMove(const std::vector<std::string>& species, 
						   const WorldManager& wm,
						   int stashcount,
						   int seed = 45843) : 
		_rand(seed), _rejected(0), _performed(0), _species(0),
		_scount(stashcount), _seed(seed)
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

			InitStashParticles(wm);
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
			auto * p = w->UnstashParticle(_species[_rand.int32() % n]);

			// Generate a random position and orientation for particle insertion.
			const auto& H = w->GetHMatrix();
			Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
			Vector3D pos = H*pr;
			p->SetPosition(pos);

			// Choose random axis, and generate random angle.
			int axis = _rand.int32() % 3 + 1;
			double deg = (4.0*_rand.doub() - 2.0)*M_PI;
			Matrix3D R = GenRotationMatrix(axis, deg);

			// Rotate particle and director.
			p->SetDirector(R*p->GetDirector());
			for(auto& child : *p)
			{
				child->SetPosition(R*(child->GetPosition()-pos) + pos);
				child->SetDirector(R*child->GetDirector());
			}

			// Insert particle.
			w->AddParticle(p);

			auto id = p->GetSpeciesID();
			auto& comp = w->GetComposition();
			auto N = comp[id];
			auto V = w->GetVolume();
			auto mu = w->GetChemicalPotential(id);
			auto lambda = w->GetWavelength(id);

			++_performed;

			// Evaluate new energy and accept/reject. 
			auto ef = ffm->EvaluateHamiltonian(*p, comp, V);

			// The acceptance rule is from Frenkel & Smit Eq. 5.6.8.
			// However,t iwas modified since we are using the *final* particle number.
			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());
			auto pacc = V/(lambda*lambda*lambda*N)*exp(beta*(mu - ef.energy.total()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				// Stashing a particle automatically removes it from world. 
				w->StashParticle(p);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(ef.energy);
				w->IncrementPressure(ef.pressure);
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
			json["type"] = "InsertParticle";
			json["stash_count"] = _scount;
			json["seed"] = _seed;

			auto& species = Particle::GetSpeciesList();
			for(auto& s : _species)
				json["species"].append(species[s]);
		}

		virtual std::string GetName() const override { return "InsertParticle"; }

		// Clone move.
		Move* Clone() const override
		{
			return new InsertParticleMove(static_cast<const InsertParticleMove&>(*this));
		}

		~InsertParticleMove(){}
	};
}