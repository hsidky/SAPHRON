#pragma once

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

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
		bool _prefac;
		int _scount; // Stash count.
		bool _multi_insert;
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

				auto* P=pcand->second->Clone();
				// Stash a characteristic amount of the particles in world.
				for(auto& world : wm)
					world->StashParticle(P, _scount);
			}
		}

	public:
		InsertParticleMove(const std::vector<int>& species, 
						   const WorldManager& wm,
						   int stashcount, bool multi_insert,
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _species(0), 
		_prefac(true), _scount(stashcount), 
		_multi_insert(multi_insert), _seed(seed)
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
						   int stashcount, bool multi_insert,
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _species(0), 
		_prefac(true), _scount(stashcount), 
		_multi_insert(multi_insert), _seed(seed)
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

			Particle* plist[32];

			int NumberofParticles=1;

			// Unstash into particle list or single particle
			if(_multi_insertion)
			{
				NumberofParticles=_species.size();
				for (int i = 0; i < _species.size(); i++)
					plist[i]=(w->UnstashParticle(_species[i]));
			}
			else
			{
				size_t n = _species.size();
				assert(n > 0);
				auto type = _rand.int32() % n;
				plist[0] = (w->UnstashParticle(_species[type]));
			}

			double Prefactor = 1;
			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());
			auto V = w->GetVolume();
			EPTuple ef;

			// Generate a random position and orientation for particle insertion.
			for (int i = 0; i < NumberofParticles; i++)
			{
				auto& p = plist[i];
				
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

				auto id = p->GetSpeciesID();
				auto& comp = w->GetComposition();
				auto N = comp[id];
				auto mu = w->GetChemicalPotential(id);
				auto lambda = w->GetWavelength(id);

				Prefactor*=V/(lambda*lambda*lambda*N)*exp(beta*mu);
			
				// Evaluate new energy for each particle. 
				// Insert particle one at a time. Done this way
				// to prevent double counting in the forcefieldmanager
				// Can be adjusted later if wated.

				w->AddParticle(p);
				ef += ffm->EvaluateHamiltonian(*p, comp, V);
			}

			++_performed;

			// The acceptance rule is from Frenkel & Smit Eq. 5.6.8.
			// However,t iwas modified since we are using the *final* particle number.
			auto pacc = Prefactor*exp(-beta*ef.energy.total());;
			pacc = pacc > 1.0 ? 1.0 : pacc;

			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				// Stashing a particle automatically removes it from world.
				for (int i = 0; i < NumberofParticles; i++)
				{
					auto& p = plist[i];
					w->StashParticle(p);
				}
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(ef.energy);
				w->IncrementPressure(ef.pressure);
			}
		}

		virtual void Perform(World* w, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{

			Particle* plist[32];
			//Evaluate initial energy and order parameter
			auto ei = w->GetEnergy();
			auto opi = op->EvaluateOrderParameter(*w);

			int NumberofParticles=1;

			// Unstash into particle list or single particle
			if(_multi_insertion)
			{
				NumberofParticles=_species.size();
				for (int i = 0; i < _species.size(); i++)
					plist[i]=(w->UnstashParticle(_species[i]));
			}
			else
			{
				size_t n = _species.size();
				assert(n > 0);
				auto type = _rand.int32() % n;
				plist[0] = (w->UnstashParticle(_species[type]));
			}

			double Prefactor = 1;
			auto& sim = SimInfo::Instance();
			auto beta = 1.0/(sim.GetkB()*w->GetTemperature());
			auto V = w->GetVolume();
			EPTuple ef;

			// Generate a random position and orientation for particle insertion.
			for (int i = 0; i < NumberofParticles; i++)
			{
				auto& p = plist[i];
				
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

				auto id = p->GetSpeciesID();
				auto& comp = w->GetComposition();
				auto N = comp[id];
				auto mu = w->GetChemicalPotential(id);
				auto lambda = w->GetWavelength(id);

				if(_prefac)
					Prefactor*=V/(lambda*lambda*lambda*N)*exp(beta*mu);
			
				// Evaluate new energy for each particle. 
				// Insert particle one at a time. Done this way
				// to prevent double counting in the forcefieldmanager
				// Can be adjusted later if wated.

				w->AddParticle(p);
				ef += ffm->EvaluateHamiltonian(*p, comp, V);
			}

			++_performed;

			// new energy update and eval OP. 
			w->IncrementEnergy(ef.energy);
			w->IncrementPressure(ef.pressure);
			auto opf = op->EvaluateOrderParameter(*w);

			// The acceptance rule is from Frenkel & Smit Eq. 5.6.8.
			// However, it iwas modified since we are using the *final* particle number.
			double pacc = op->AcceptanceProbability(ei, ef.energy, opi, opf, *w);
			
			// If prefactor is enabled, compute.
			if(_prefac)
				pacc *= Prefactor;

			pacc = pacc > 1.0 ? 1.0 : pacc;

			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				// Stashing a particle automatically removes it from world. 
				for (int i = 0; i < NumberofParticles; i++)
				{
					auto& p = plist[i];
					w->StashParticle(p);
				}
				w->IncrementEnergy(-1.0*ef.energy);
				w->IncrementPressure(-1.0*ef.pressure);
				++_rejected;
			}
			
		}

		// Turn on or off the acceptance rule prefactor 
		// for DOS order parameter.
		void SetOrderParameterPrefactor(bool flag) { _prefac = flag; }

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
			json["multi_insertion"] = _multi_insertion;
			json["seed"] = _seed;
			json["op_prefactor"] = _prefac;

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