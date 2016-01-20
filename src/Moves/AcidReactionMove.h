#pragma once 

#include "Move.h"
#include "../Utils/Helpers.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

/* Currently move only works with acid where the following is the reaction:
forward HA -> A- + H+
Reverse A- + H+ -> HA
*/

namespace SAPHRON
{
	class AcidReactionMove : public Move
	{
	private:
		std::vector<int> _swap;
		std::vector<int> _products;
		Rand _rand;

		int _performed;
		int _rejected;
		bool _prefac;
		int _scount;
		double _pKo;

		double _m1;
		double _m2;

		int _i1;
		int _i2;

		double _c1;
		double _c2;

		int _seed;

		void InitStashParticles(const WorldManager& wm)
		{
			// Get particle map, find one of the appropriate species 
			// and clone.
			auto& plist = Particle::GetParticleMap();
			for(auto& id : _products)
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
		AcidReactionMove(const std::vector<std::string>& swap,
		const std::vector<std::string>& products,
		const WorldManager& wm,
		int stashcount, double pKo,int seed = 7456253) : 
		_swap(0),_products(0), _rand(seed), _performed(0),
		 _rejected(0), _prefac(true), _scount(stashcount),
		 _pKo(pKo), _m1(0), _m2(0), _i1(0), _i2(1), _c1(0),
		 _c2(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();

			for(auto& id : swap)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_swap.push_back(it - list.begin());
			}

			for(auto& id : products)
			{
				auto it = std::find(list.begin(), list.end(), id);
				if(it == list.end())
				{
					std::cerr << "Species ID \""
							  << id << "\" provided does not exist."
							  << std::endl;
					exit(-1);
				}
				_products.push_back(it - list.begin());
			}

			InitStashParticles(wm);

			auto& plist = Particle::GetParticleMap();

			auto id =_swap[0];
			auto p1 = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);
			id =_swap[1];			
			auto p2 = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);
			
			_c1=p1->second->GetCharge();
			_c2=p2->second->GetCharge();

			_m1=p1->second->GetMass();
			_m2=p2->second->GetMass();

			_i1 = p1->second->GetSpeciesID();
			_i2 = p2->second->GetSpeciesID();

		}

		AcidReactionMove(const std::vector<int>& swap,
		const std::vector<int>& products,
		const WorldManager& wm,
		int stashcount, double pKo,int seed = 7456253) : 
		_swap(0),_products(0), _rand(seed), _performed(0),
		 _rejected(0), _prefac(true), _scount(stashcount),
		 _pKo(pKo), _m1(0), _m2(0), _i1(0), _i2(1), _c1(0),
		 _c2(0), _seed(seed)
		{
			// Verify species list and add to local vector.
			auto& list = Particle::GetSpeciesList();
			
			for(auto& id : swap)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_swap.push_back(id);
			}
			
			for(auto& id : products)
			{
				if(id >= (int)list.size())
				{
					std::cerr << "Species ID \"" 
							  << id << "\" provided does not exist." 
							  << std::endl;
					exit(-1);
				}
				_products.push_back(id);
			}

			InitStashParticles(wm);

			auto& plist = Particle::GetParticleMap();

			auto id =_swap[0];
			auto p1 = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);
			id =_swap[1];			
			auto p2 = std::find_if(plist.begin(), plist.end(), 
					[=](const std::pair<int, Particle*>& p)
					{
						return p.second->GetSpeciesID() == id;
					}
				);
			
			_c1=p1->second->GetCharge();
			_c2=p2->second->GetCharge();

			_m1=p1->second->GetMass();
			_m2=p2->second->GetMass();

			_i1 = p1->second->GetSpeciesID();
			_i2 = p2->second->GetSpeciesID();
		} 

		virtual void Perform(WorldManager* wm, 
					 ForceFieldManager* ffm, 
					 const MoveOverride& override) override
		{

			// Draw random particle from random world.
			auto* w = wm->GetRandomWorld();
			Particle* p1 = nullptr;
			Particle* p2 = nullptr;
			Particle* ph = nullptr;

			int RxnExtent = 1;
			double rxndirection = _rand.doub();

			//Determine reaction direction.
			if(rxndirection>=0.5) //Forward reaction
			{
				RxnExtent = 1;
				p1=w->DrawRandomPrimitiveBySpecies(_swap[0]);

				if(p1==nullptr)
					return;
			}

			else //Reverse reaction
			{
				RxnExtent = -1;
				p2 = w->DrawRandomPrimitiveBySpecies(_swap[1]);
				ph = w->DrawRandomPrimitiveBySpecies(_products[0]);
	
				if(ph==nullptr && p2==nullptr)
					return;
				
				else if(ph==nullptr || p2==nullptr)
				{
					std::cout<<"Only Half of the products present! Exiting"<<std::endl;
					exit(-1);
				}
			}

			double lambdaratio;

			auto& comp = w->GetComposition();
			int comp1 = comp[_i1];
			int comp2 = comp[_i2];
			int compph = comp[_products[0]];

			auto V = pow(w->GetVolume(),RxnExtent);
			auto lambda = w->GetWavelength(_products[0]);
			auto lambda3 = pow(lambda*lambda*lambda,-1*RxnExtent);
			double Nratio=0;
			double Korxn=0;

			// Evaluate initial energy. 
			EPTuple ei, ef;

			if(RxnExtent == -1)
			{
				Nratio = comp2*compph/(comp1 + 1.0);
				Korxn = exp(_pKo);
				ei = ffm->EvaluateHamiltonian(*ph, w->GetComposition(), w->GetVolume());
				w->RemoveParticle(ph);
				ei += ffm->EvaluateHamiltonian(*p2, w->GetComposition(), w->GetVolume());
				p2->SetCharge(_c1);
				p2->SetMass(_m1);
				p2->SetSpeciesID(_i1);

				ef = ffm->EvaluateHamiltonian(*p2, w->GetComposition(), w->GetVolume());
				lambdaratio = pow(_m1/_m2,3.0/2.0);
			}

			else
			{
				Nratio = comp1/((comp2+1.0)*(compph+1.0));
				Korxn = exp(-_pKo);

				ei = ffm->EvaluateHamiltonian(*p1, w->GetComposition(), w->GetVolume());
				
				p1->SetCharge(_c2);
				p1->SetMass(_m2);
				p1->SetSpeciesID(_i2);
				
				ef = ffm->EvaluateHamiltonian(*p1, w->GetComposition(), w->GetVolume());
				
				ph = w->UnstashParticle(_products[0]);
				// Generate a random position and orientation for particle insertion.
				const auto& H = w->GetHMatrix();
				Vector3D pr{_rand.doub(), _rand.doub(), _rand.doub()};
				Vector3D pos = H*pr;
				ph->SetPosition(pos);

				// Insert particle.
				w->AddParticle(ph);
				ef += ffm->EvaluateHamiltonian(*ph, w->GetComposition(), w->GetVolume());
				lambdaratio = pow(_m2/_m1,3.0/2.0);
			}

			++_performed;

			auto de = ef - ei;
		
			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability.
			double pacc = Nratio*V*lambda3*lambdaratio*Korxn*
			exp((-de.energy.total())/(w->GetTemperature()*sim.GetkB()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				++_rejected;
				if(RxnExtent == -1) 
				{
					p2->SetCharge(_c2);
					p2->SetMass(_m2);
					p2->SetSpeciesID(_i2);
					w->AddParticle(ph);
				}

				else 
				{
					p1->SetCharge(_c1);
					p1->SetMass(_m1);
					p1->SetSpeciesID(_i1);
					w->RemoveParticle(ph);
					w->StashParticle(ph);
				}

			}

			else
			{
				if(RxnExtent == -1)
				{
					w->StashParticle(ph);
				}

				w->IncrementEnergy(de.energy);
				w->IncrementPressure(de.pressure);
			}
			
		}

		virtual void Perform(World* world, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{
			std::cerr << "Acid Reaction move does not support DOS interface." << std::endl;
			exit(-1);
		}

		// Turns on or off the acceptance rule prefactor for DOS order parameter.
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
			json["type"] = GetName();
			json["seed"] = _seed;
			json["pKo"] = _pKo;
			json["stash_count"] = _scount;
			json["op_prefactor"] = _prefac;

			auto& slist = Particle::GetSpeciesList();
			for(auto& s : _swap)
				json["swap"].append(slist[s]);
			for(auto& s : _products)
				json["products"].append(slist[s]);
		}

		virtual std::string GetName() const override { return "AcidReaction"; }

		// Clone move.
		Move* Clone() const override
		{
			return new AcidReactionMove(static_cast<const AcidReactionMove&>(*this));
		}

	};
}