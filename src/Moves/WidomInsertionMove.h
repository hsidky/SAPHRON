#pragma once

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

namespace SAPHRON
{
	// Class for widom move. Based on providing 
	// a list of species, the move will create _ghost copies 
	// of each one.
	class WidomInsertionMove : public Move
	{
	private: 
		Rand _rand;
		int _rejected;
		int _performed;
		ParticleList _ghosts;

		////////Not Thread Safe//////////////////
		double _sum_energies;
		/////////////////////////////////////////

		int _seed;

		void InitGhostParticle(std::vector<int> IDs)
		{
			// Get particle map, find one of the appropriate species 
			// and clone. Can't clone if none available!
			auto& plist = Particle::GetParticleMap();
			for(auto& id : IDs)
			{
				// Since the species exists, we assume there must be at least 
				// one find. 
				auto pcand = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);

				//Create the ghost particles to be used
				_ghosts.push_back(pcand->second->Clone());
			}
		}

	public:
		WidomInsertionMove(const std::vector<int>& species, 
						   const WorldManager& wm,
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _ghosts(), 
		_sum_energies(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			std::vector<int> IDs;
			for(auto& id : species)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				IDs.push_back(id);
			}
			InitGhostParticle(IDs);
		}

		WidomInsertionMove(const std::vector<std::string>& species, 
						   const WorldManager& wm,
						   int seed = 45843) :
		_rand(seed), _rejected(0), _performed(0), _ghosts(), 
		_sum_energies(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			std::vector<int> IDs;
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
				IDs.push_back(it - list.begin());
			}
			InitGhostParticle(IDs);
		}

		virtual void Perform(WorldManager* wm, 
							 ForceFieldManager* ffm, 
							 const MoveOverride& override) override
		{
			// Get random world.
			World* w = wm->GetRandomWorld();
			EPTuple ef;
			auto& comp = w->GetComposition();
			auto V = w->GetVolume();

			// Generate a random position and orientation for particle insertion.
			for (auto& p : _ghosts)
			{
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
			
				w->AddParticle(p);
				ef += ffm->EvaluateHamiltonian(*p, comp, V);
			}

			auto& sim = SimInfo::Instance();
			auto KbT = sim.GetkB()*w->GetTemperature();
			auto beta = 1.0/(KbT);
			_sum_energies += exp(-beta*ef.energy.total());
			++_performed;
			auto mu = -KbT*log(_sum_energies/_performed);

			for (auto& p : _ghosts)
			{
				w->SetChemicalPotential(mu,p->GetSpeciesID());
				w->RemoveParticle(p);				
			}

		}

		virtual void Perform(World* w, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{
			std::cerr << "Widom insertion move does not support DOS interface." << std::endl;
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
			_sum_energies = 0;
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "WidomInsertion";
			json["seed"] = _seed;

			auto& species = Particle::GetSpeciesList();
			for(auto& p : _ghosts)
				json["species"].append(species[p->GetSpeciesID()]);
		}

		virtual std::string GetName() const override { return "WidomInsertion"; }

		// Clone move.
		Move* Clone() const override
		{
			return new WidomInsertionMove(static_cast<const WidomInsertionMove&>(*this));
		}

		~WidomInsertionMove()
		{
			for (auto& p : _ghosts)
				delete p;
			_ghosts.clear();
		}
	};
}