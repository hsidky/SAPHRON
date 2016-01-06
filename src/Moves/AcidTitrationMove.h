#pragma once 
#include "Move.h"
#include "../Particles/Particle.h"
#include "../Worlds/WorldManager.h"
#include "../Worlds/World.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Utils/Rand.h"
#include "../DensityOfStates/DOSOrderParameter.h"

namespace SAPHRON
{
	class AcidTitrationMove : public Move
	{
	private:
		std::vector<int> _species;
		Rand _rand;

		int _performed;
		int _rejected;
		int _seed;

		double _protoncharge;
		double _mu;

		bool _prefac;

		std::string _mapkey;

	public:
		AcidTitrationMove(const std::vector<std::string>& species,
		double protoncharge, double mu, int seed = 7456253) : 
		_species(0), _rand(seed), _performed(0), _rejected(0),
		_seed(seed), _protoncharge(protoncharge), _mu(mu),
		_prefac(true), _mapkey("conjugateform")
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

		AcidTitrationMove(const std::vector<int>& species,
		double protoncharge, double mu, int seed = 7456253) : 
		_species(0), _rand(seed), _performed(0), _rejected(0),
		_seed(seed), _protoncharge(protoncharge), _mu(mu),
		_prefac(true), _mapkey("conjugateform")
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

		virtual void Perform(WorldManager* wm, 
					 ForceFieldManager* ffm, 
					 const MoveOverride& override) override
		{
			// Draw random particle from random world.
			auto* w = wm->GetRandomWorld();
			auto* p = w->DrawRandomParticle();

			if(p == nullptr)
				return;

			if(std::find(_species.begin(), _species.end(), p->GetSpeciesID()) == _species.end())
				return;

			// Particle must have at least two children for this move.
			auto& children = p->GetChildren();
			auto n = children.size();

			// Draw a random child.
			auto n1 = _rand.int32() % n;

			// Get protonation state.
			auto* p1 = children[n1];

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateHamiltonian(*p, w->GetComposition(), w->GetVolume());

			// Perform protonation/deprotonation.
			auto tempc=p1->GetCharge();
			auto conjugateflag = p1->GetTag(_mapkey);
			auto adjustedmu = _mu;

			//Deprotonate acid if protonated
			if(!conjugateflag)
			{
				p1->SetCharge(tempc - _protoncharge);
				p1->SetTag(_mapkey,false);
				adjustedmu = -_mu;
			}

			//Protonate if deprotonated
			else
			{
				p1->SetCharge(tempc + _protoncharge);
				p1->SetTag(_mapkey,true);
			}

			++_performed;

			auto ef = ffm->EvaluateHamiltonian(*p, w->GetComposition(), w->GetVolume());
			auto de = ef - ei;
			
			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability.
			double pacc = exp((-de.energy.total()-adjustedmu)/(w->GetTemperature()*sim.GetkB()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				p1->SetCharge(tempc);
				p1->SetTag(_mapkey,conjugateflag);
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de.energy);
				w->IncrementPressure(de.pressure);
			}
		}

		// Perform move interface for DOS ensemble.
		virtual void Perform(World* world, 
							 ForceFieldManager* ffm, 
							 DOSOrderParameter* op, 
							 const MoveOverride& override) override
		{

			auto* p = world->DrawRandomParticle();

			if(p == nullptr)
				return;

			if(std::find(_species.begin(), _species.end(), p->GetSpeciesID()) == _species.end())
				return;

			// Particle must have at least two children for this move.
			auto& children = p->GetChildren();
			auto n = children.size();

			// Draw a random child.
			auto n1 = _rand.int32() % n;

			// Get protonation state.
			auto* p1 = children[n1];

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateHamiltonian(*p, world->GetComposition(), world->GetVolume());
			auto opi = op->EvaluateOrderParameter(*world);

			// Perform protonation/deprotonation.
			auto tempc=p1->GetCharge();
			auto conjugateflag = p1->GetTag(_mapkey);
			auto adjustedmu = _mu;

			//Deprotonate acid if protonated
			if(!conjugateflag)
			{
				p1->SetCharge(tempc - _protoncharge);
				p1->SetTag(_mapkey,false);
				adjustedmu = -_mu;
			}

			//Protonate if deprotonated
			else
			{
				p1->SetCharge(tempc + _protoncharge);
				p1->SetTag(_mapkey,true);
			}

			++_performed;

			auto ef = ffm->EvaluateHamiltonian(*p, world->GetComposition(), world->GetVolume());
			auto de = ef - ei;

			// Update energies and pressures.
			world->IncrementEnergy(de.energy);
			world->IncrementPressure(de.pressure);

			auto opf = op->EvaluateOrderParameter(*world);
			
			// Acceptance probability.
			double pacc = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *world);
			if(_prefac)
			{
				auto& sim = SimInfo::Instance();
				auto beta = 1.0/(world->GetTemperature()*sim.GetkB());
				auto arg = beta*(-adjustedmu);
				pacc *= exp(arg);
			}
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				p1->SetCharge(tempc);
				p1->SetTag(_mapkey,conjugateflag);
				++_rejected;
			}
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
			json["mu"] = _mu;
			json["proton_charge"] = _protoncharge;
			json["op_prefactor"] = _prefac;
			json["map_key"] = _mapkey;

			for(auto& s : _species)
				json["species"].append(s);
		}

		virtual std::string GetName() const override { return "Titration"; }

		// Clone move.
		Move* Clone() const override
		{
			return new AcidTitrationMove(static_cast<const AcidTitrationMove&>(*this));
		}

	};
}