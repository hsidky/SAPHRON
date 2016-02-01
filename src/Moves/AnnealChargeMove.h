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
	class AnnealChargeMove : public Move
	{
	private:
		std::vector<int> _species;
		Rand _rand;

		int _performed;
		int _rejected;
		int _seed;

	public:
		AnnealChargeMove(const std::vector<std::string>& species, int seed = 2474) : 
		_species(0), _rand(seed), _performed(0), _rejected(0), _seed(seed)
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

		AnnealChargeMove(const std::vector<int>& species, int seed = 438765) :
		_species(0), _rand(seed), _performed(0), _rejected(0), _seed(seed)
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

		//No Double counting problem.
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
			if(n < 2)
			{
				std::cerr << "Particle type " << p->GetSpecies() << " must have at least two "
				"children. Cannot perform charge annealing move" << std::endl;
				exit(-1);
			}

			// Draw two random children that are unique.
			auto n1 = _rand.int32() % n;
			auto n2 = _rand.int32() % n;
			while(n2 == n1)
				n2 = _rand.int32() % n;

			// Get charges.
			auto* p1 = children[n1];
			auto* p2  = children[n2];
			auto c1 = p1->GetCharge();
			auto c2 = p2->GetCharge();

			// If charges are equal just return.
			if(c1 == c2)
				return;

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateEnergy(*p1);
			ei+= ffm->EvaluateEnergy(*p2);
			// Perform charge swap.
			p1->SetCharge(c2);
			p2->SetCharge(c1);
			++_performed;

			auto ef = ffm->EvaluateEnergy(*p1);
			ef += ffm->EvaluateEnergy(*p2);
			auto de = ef - ei;
			
			// Get sim info for kB.
			auto& sim = SimInfo::Instance();

			// Acceptance probability.
			double pacc = exp(-de.energy.total()/(w->GetTemperature()*sim.GetkB()));
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				p1->SetCharge(c1);
				p2->SetCharge(c2);
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
			if(n < 2)
			{
				std::cerr << "Particle type " << p->GetSpecies() << " must have at least two "
				"children. Cannot perform charge annealing move" << std::endl;
				exit(-1);
			}

			// Draw two random children that are unique.
			auto n1 = _rand.int32() % n;
			auto n2 = _rand.int32() % n;
			while(n2 == n1)
				n2 = _rand.int32() % n;

			// Get charges.
			auto* p1 = children[n1];
			auto* p2  = children[n2];
			auto c1 = p1->GetCharge();
			auto c2 = p2->GetCharge();

			// If charges are equal just return.
			if(c1 == c2)
				return;

			// Evaluate initial energy. 
			auto ei = ffm->EvaluateEnergy(*p1);
			ei += ffm->EvaluateEnergy(*p2);
			auto opi = op->EvaluateOrderParameter(*world);

			// Perform charge swap.
			p1->SetCharge(c2);
			p2->SetCharge(c1);
			++_performed;

			auto ef = ffm->EvaluateEnergy(*p1);
			ef += ffm->EvaluateEnergy(*p2);
			auto de = ef - ei;

			// Update energies and pressures.
			world->IncrementEnergy(de.energy);
			world->IncrementPressure(de.pressure);

			auto opf = op->EvaluateOrderParameter(*world);
			
			// Acceptance probability.
			double pacc = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *world);
			pacc = pacc > 1.0 ? 1.0 : pacc;

			// Reject or accept move.
			if(!(override == ForceAccept) && (pacc < _rand.doub() || override == ForceReject))
			{
				p1->SetCharge(c1);
				p2->SetCharge(c2);
				++_rejected;
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

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = GetName();
			json["seed"] = _seed;
			auto& slist = Particle::GetSpeciesList();
			for(auto& s : _species)
				json["species"].append(slist[s]);
		}

		virtual std::string GetName() const override { return "AnnealCharge"; }

		// Clone move.
		Move* Clone() const override
		{
			return new AnnealChargeMove(static_cast<const AnnealChargeMove&>(*this));
		}

	};
}