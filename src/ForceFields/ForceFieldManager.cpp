#include "ForceFieldManager.h"
#include "../Constraints/Constraint.h"
#include "config.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <stdexcept>

namespace SAPHRON
{
	// Adds a forcefield to the manager.
	void ForceFieldManager::AddNonBondedForceField(std::string p1type, std::string p2type, ForceField& ff)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);

		if(p1 == list.end() || p2 == list.end())
			throw std::invalid_argument("Unknown particle type(s). Please register particles\n"
						          		"   before adding the forcefield to the manager.");		          

		AddNonBondedForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddNonBondedForceField(int p1type, int p2type, ForceField& ff)
	{
		_nonbondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_nonbondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniquenbffs.find({p1type, p2type}) == _uniquenbffs.end() && 
			_uniquenbffs.find({p2type, p1type}) == _uniquenbffs.end())
			_uniquenbffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveNonBondedForceField(std::string p1type, std::string p2type)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);
		if(p1 != list.end() && p2 != list.end())
			RemoveNonBondedForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveNonBondedForceField(int p1type, int p2type)
	{
		_nonbondedforcefields.erase({p1type, p2type});
		_nonbondedforcefields.erase({p2type, p1type});

		_uniquenbffs.erase({p1type, p2type});
		_uniquenbffs.erase({p2type, p1type});

	}

	// Get the number of registered forcefields.
	int ForceFieldManager::NonBondedForceFieldCount()
	{
		return _uniquenbffs.size();
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddBondedForceField(std::string p1type, std::string p2type, ForceField& ff)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);

		if(p1 == list.end() || p2 == list.end())
			throw std::invalid_argument("Unknown particle type(s). Please register particles\n"
						          		"   before adding the forcefield to the manager.");	

		AddBondedForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddBondedForceField(int p1type, int p2type, ForceField& ff)
	{
		_bondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_bondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniquebffs.find({p1type, p2type}) == _uniquebffs.end() && 
			_uniquebffs.find({p2type, p1type}) == _uniquebffs.end())
			_uniquebffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveBondedForceField(std::string p1type, std::string p2type)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);
		
		if(p1 != list.end() && p2 != list.end())
			RemoveBondedForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveBondedForceField(int p1type, int p2type)
	{
		_bondedforcefields.erase({p1type, p2type});
		_bondedforcefields.erase({p2type, p1type});

		_uniquebffs.erase({p1type, p2type});
		_uniquebffs.erase({p2type, p1type});

	}

	// Get the number of registered forcefields.
	int ForceFieldManager::BondedForceFieldCount()
	{
		return _uniquebffs.size();
	}

	// Sets the electrostatic forcefield.
	void ForceFieldManager::SetElectrostaticForcefield(const ForceField &ff)
	{
		_electroff = &ff;
	}

	// Resets the electrostatic forcefield.
	void ForceFieldManager::ResetElectrostaticForceField()
	{
		_electroff = nullptr;
	}

	// Add a constraint to a world.
	void ForceFieldManager::AddConstraint(const World& world, Constraint* cc)
	{
		auto id = world.GetID();
		if((int)_constraints.size() - 1 < id)
			_constraints.resize(id + 1);

		_constraints[id].push_back(cc);
	}
	
	// Resets constraints on a world.
	void ForceFieldManager::ResetConstraints(const World& world)
	{
		auto id = world.GetID();
		if((int)_constraints.size() - 1 >= id)
			_constraints[id].clear();
	}

	double ForceFieldManager::EvaluateConstraintEnergy(const World& world) const
	{
		auto energy = 0.;
		auto id = world.GetID();
		
		if((int)_constraints.size() - 1 >= id)
			for(auto& c : _constraints[id])
				energy += c->EvaluateEnergy();

		return energy;
	}

	EPTuple ForceFieldManager::EvaluateInterEnergy(const Particle& particle) const
	{
		double intere = 0, electroe = 0, pxx = 0, pxy = 0, pxz = 0, pyy = 0, pyz = 0, pzz = 0;

		// Begin timer.
		auto& sim = SimInfo::Instance();
		sim.StartTimer("e_inter");

		// Get appropriate world and ID.
		World* world = particle.GetWorld();
		if(!particle.HasChildren())
		{
			unsigned wid = (world == nullptr) ? 0 : world->GetID();
			auto& neighbors = particle.GetNeighbors();
			auto n = neighbors.size();

			#ifdef PARALLEL_INTER
			#pragma omp parallel for reduction(+:intere,electroe,pxx,pxy,pxz,pyy,pyz,pzz) if(n >= MIN_INTER_NEIGH)
			#endif
			for(size_t k = 0; k < n; ++k)
			{
				auto* neighbor = neighbors[k];

				Position rij = particle.GetPosition() - neighbor->GetPosition();
											
				if(world != nullptr)
					world->ApplyMinimumImage(&rij);

				// If particle has parent, compute vector between parent Particle(s).
				Position rab = rij;
				if(particle.HasParent() && neighbor->HasParent())
				{
					rab = particle.GetParent()->GetPosition() - neighbor->GetParent()->GetPosition();
					if(world != nullptr)
						world->ApplyMinimumImage(&rab);

				}
				else if(neighbor->HasParent() && !particle.HasParent()) 
				{
					rab = particle.GetPosition() - neighbor->GetParent()->GetPosition();
					if(world != nullptr)
						world->ApplyMinimumImage(&rab);
				}
				else if(!neighbor->HasParent() && particle.HasParent()) {
					rab = particle.GetParent()->GetPosition() - neighbor->GetPosition();
					if(world != nullptr)
						world->ApplyMinimumImage(&rab);
				}

				auto it = _nonbondedforcefields.find({particle.GetSpeciesID(),neighbor->GetSpeciesID()});

				Interaction interij, electroij;

				// Interaction containing energy and virial.
				if(it != _nonbondedforcefields.end())
				{
					auto* ff = it->second;
					interij = ff->Evaluate(particle, *neighbor, rij, wid);
				}

				//Electrostatics containing energy and virial
				if(_electroff != nullptr)
					electroij = _electroff->Evaluate(particle, *neighbor, rij, wid);
				
				intere += interij.energy; // Sum nonbonded van der Waal energy.
				electroe += electroij.energy; // Sum electrostatic energy

				auto totalvirial = interij.virial + electroij.virial;
				
				pxx += totalvirial * rij[0] * rab[0];
				pyy += totalvirial * rij[1] * rab[1];
				pzz += totalvirial * rij[2] * rab[2];
				pxy += totalvirial * 0.5 * (rij[0] * rab[1] + rij[1] * rab[0]);
				pxz += totalvirial * 0.5 * (rij[0] * rab[2] + rij[2] * rab[0]);
				pyz += totalvirial * 0.5 * (rij[1] * rab[2] + rij[2] * rab[1]);				
			}
		}
		EPTuple ep{intere, 0, electroe, 0, 0, 0, 0, 0, 0, -pxx, -pxy, -pxz, -pyy, -pyz, -pzz, 0};				
		
		// End timer.
		sim.AddTime("e_inter");
		
		for(auto& child : particle)
			ep += EvaluateInterEnergy(*child);	
		
		// Divide virial by volume to get pressure if there's a world.
		if(world != nullptr)
		ep.pressure /= world->GetVolume();

		return ep;
	}

	EPTuple ForceFieldManager::EvaluateInterEnergy(const World& world) const
	{
		EPTuple ep; 
		for(auto& particle : world)
			ep += EvaluateInterEnergy(*particle);

		ep.energy.intervdw *= 0.5;
		ep.energy.interelectrostatic *= 0.5;
		ep.pressure *= 0.5;
		return ep;
	}

	EPTuple ForceFieldManager::EvaluateIntraEnergy(const World& world) const
	{
		EPTuple ep; 
		for(auto& particle : world)
			ep += EvaluateIntraEnergy(*particle);

		return ep;
	}

	EPTuple ForceFieldManager::EvaluateIntraEnergy(const Particle& particle) const
	{
		EPTuple ep;

		// Begin timer.
		auto& sim = SimInfo::Instance();
		sim.StartTimer("e_intra");
		
		World* world = particle.GetWorld();
		unsigned int wid = (world == nullptr) ? 0 : world->GetID();

		// Go to particle parent and evaluate non-bonded interactions with siblings.
        if(particle.HasParent())
        { 
			double electro = 0, vdw = 0;
        	auto& siblings = particle.GetParent()->GetChildren();
        	auto n = siblings.size();

        	#ifdef PARALLEL_INTRA
        	#pragma omp parallel for reduction(+:electro,vdw) if(n >= MIN_INTRA_NEIGH)
        	#endif
            for(size_t i = 0; i < n; ++i)
            {
            	auto& sibling = siblings[i];
                if(!particle.IsBondedNeighbor(sibling) && sibling != &particle)
                {
                	
                	Position rij = particle.GetPosition() - sibling->GetPosition();
					
					if(world != nullptr)
						world->ApplyMinimumImage(&rij);

					auto it = _nonbondedforcefields.find({particle.GetSpeciesID(), sibling->GetSpeciesID()});

					//Electrostatics containing energy and virial
					if(_electroff != nullptr)
					{
						auto ij = _electroff->Evaluate(particle, *sibling, rij, wid);
						electro += ij.energy;
					}		

					if(it != _nonbondedforcefields.end())
					{
						auto* ff = it->second;
						auto ij = ff->Evaluate(particle, *sibling, rij, wid);

						vdw += ij.energy; // Sum nonbonded energy.
					}
                }
            }

            ep.energy.intraelectrostatic += electro;
            ep.energy.intravdw += vdw;
        }

        for(auto* bondedneighbor : particle.GetBondedNeighbors())
		{
			auto it = _bondedforcefields.find({particle.GetSpeciesID(),bondedneighbor->GetSpeciesID()});
			if(it != _bondedforcefields.end())
			{
				auto ff = it->second;
				Position rij = particle.GetPosition() - bondedneighbor->GetPosition();
				
				// Minimum image convention.
				if(world != nullptr)
					world->ApplyMinimumImage(&rij);

				auto ij = ff->Evaluate(particle, *bondedneighbor, rij, wid);
				ep.energy.bonded += ij.energy; // Sum bonded energy.
			}
		}

		for(auto& c : particle.GetConnectivities())
			ep.energy.connectivity += c->EvaluateEnergy(particle);
		
		// End timer.
		sim.AddTime("e_intra");
		
		for(auto& child : particle)
			ep += 0.5*EvaluateIntraEnergy(*child);

		return ep;
	}

	EPTuple ForceFieldManager::EvaluateTailEnergy(const World& world) const
	{
		auto& comp = world.GetComposition();
		auto wid = world.GetID();
		auto volume = world.GetVolume();
		EPTuple ep;

		for(auto& it : _nonbondedforcefields)
		{
			const auto& pair = it.first;
			const auto& ff = it.second;

			// Get species compositions.
			auto na = comp[pair.first];
			auto nb = comp[pair.second];

			ep.energy.tail += 2.0*M_PI*na*nb*ff->EnergyTailCorrection(wid);
			ep.pressure.ptail += 2.0/3.0*M_PI*na*nb*ff->PressureTailCorrection(wid);
		}

		ep.energy.tail /= volume;
		ep.pressure.ptail /= volume*volume;

		return ep;
	}

	void ForceFieldManager::Serialize(Json::Value& json) const
	{
		auto& species = Particle::GetSpeciesList();

		// Go through non-bonded FF.
		if(_uniquenbffs.size() != 0)
		{
			auto& nonbonded = json["forcefields"]["nonbonded"];
			for(auto& ff : _uniquenbffs)
			{
				auto& last = nonbonded[nonbonded.size()];
				ff.second->Serialize(last);
				auto& pair = ff.first;
				last["species"][0] = species[pair.first];
				last["species"][1] = species[pair.second];
			}
		}

		if(_uniquebffs.size() != 0)
		{
			auto& bonded = json["forcefields"]["bonded"];
			for(auto& ff : _uniquebffs)
			{
				auto& pair = ff.first;
				auto& last = bonded[bonded.size()];
				ff.second->Serialize(last);
				last["species"][0] = species[pair.first];
				last["species"][1] = species[pair.second];
			}
		}

		if(_electroff != nullptr)
		{
			auto& electro = json["forcefields"]["electrostatic"];
			_electroff->Serialize(electro);
		}

		if(_constraints.size() != 0)
		{
			auto& constraints = json["forcefields"]["constraints"];
			for(size_t i = 0; i < _constraints.size(); ++i)
			{
				auto& clist = _constraints[i];
				for(auto& c : clist)
				{
					auto& last = constraints[constraints.size()];
					c->Serialize(last);
					last["world"] = static_cast<int>(i);
				}
			}
		}
	}

	EPTuple ForceFieldManager::EvaluateEnergy(const Particle& particle) const
	{
		return EvaluateInterEnergy(particle) + EvaluateIntraEnergy(particle);
	}

	EPTuple ForceFieldManager::EvaluateEnergy(const World& world) const
	{
		auto e = EvaluateInterEnergy(world) + EvaluateIntraEnergy(world) + EvaluateTailEnergy(world);
		e.energy.constraint = EvaluateConstraintEnergy(world);
		return e;
	}
}