#include "ForceFieldManager.h"
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

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddElectrostaticForceField(std::string p1type, std::string p2type, ForceField& ff)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);

		if(p1 == list.end() || p2 == list.end())
			throw std::invalid_argument("Unknown particle type(s). Please register particles\n"
						          		"   before adding the forcefield to the manager.");	

		AddElectrostaticForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddElectrostaticForceField(int p1type, int p2type, ForceField& ff)
	{
		_electrostaticforcefield.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_electrostaticforcefield.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniqueeffs.find({p1type, p2type}) == _uniqueeffs.end() && 
			_uniqueeffs.find({p2type, p1type}) == _uniqueeffs.end())
			_uniqueeffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveElectrostaticForceField(std::string p1type, std::string p2type)
	{
		auto list = Particle::GetSpeciesList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);
		if(p1 != list.end() && p2 != list.end())
			RemoveElectrostaticForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveElectrostaticForceField(int p1type, int p2type)
	{
		_electrostaticforcefield.erase({p1type, p2type});
		_electrostaticforcefield.erase({p2type, p1type});

		_uniqueeffs.erase({p1type, p2type});
		_uniqueeffs.erase({p2type, p1type});
	}

	// Get the number of registered forcefields.
	int ForceFieldManager::ElectrostaticForceFieldCount()
	{
		return _uniqueeffs.size();
	}

	EPTuple ForceFieldManager::EvaluateInterEnergy(const Particle& particle) const
	{
		double intere = 0, electroe = 0, pxx = 0, pxy = 0, pxz = 0, pyy = 0, pyz = 0, pzz = 0;

		// Begin timer.
		auto& sim = SimInfo::Instance();
		sim.StartTimer("e_inter");

		// Get appropriate world and ID.
		if(!particle.HasChildren())
		{
			World* world = particle.GetWorld();
			unsigned int wid = (world == nullptr) ? 0 : world->GetID();
			auto& neighbors = particle.GetNeighbors();

			#pragma omp parallel for reduction(+:intere,electroe,pxx,pxy,pxz,pyy,pyz,pzz)
			for(size_t k = 0; k < neighbors.size(); ++k)
			{
				auto* neighbor = neighbors[k];

				Position rij = particle.GetPosition() - neighbor->GetPosition();
											
				if(world != nullptr)
					world->ApplyMinimumImage(&rij);

				// If particle has parent, compute vector between parent molecule(s).
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
				auto it2 = _electrostaticforcefield.find({particle.GetSpeciesID(),neighbor->GetSpeciesID()});

				Interaction interij, electroij;

				// Interaction containing energy and virial.
				if(it != _nonbondedforcefields.end())
				{
					auto* ff = it->second;
					interij = ff->Evaluate(particle, *neighbor, rij, wid);
				}

				//Electrostatics containing energy and virial
				if(it2 != _electrostaticforcefield.end())
				{
					auto* eff = it2->second;
					electroij = eff->Evaluate(particle, *neighbor, rij, wid);
				}
				
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
		EPTuple ep{intere, 0, electroe, 0, 0, 0, 0, 0, -pxx, -pxy, -pxz, -pyy, -pyz, -pzz, 0};				
		
		// End timer.
		sim.AddTime("e_inter");
		
		for(auto& child : particle)
			ep += EvaluateInterEnergy(*child);	
		
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
            for(auto& sibling : *particle.GetParent())
            {
                if(!particle.IsBondedNeighbor(sibling) && sibling != &particle)
                {
                	
                	Position rij = particle.GetPosition() - sibling->GetPosition();
					
					if(world != nullptr)
						world->ApplyMinimumImage(&rij);

					auto it = _nonbondedforcefields.find({particle.GetSpeciesID(), sibling->GetSpeciesID()});
					auto it2 = _electrostaticforcefield.find({particle.GetSpeciesID(), sibling->GetSpeciesID()});

					//Electrostatics containing energy and virial
					if(it2 != _electrostaticforcefield.end())
					{
						auto* eff = it2->second;
						auto ij = eff->Evaluate(particle, *sibling, rij, wid);
						ep.energy.intraelectrostatic += ij.energy;
					}		

					if(it != _nonbondedforcefields.end())
					{
						auto* ff = it->second;
						auto ij = ff->Evaluate(particle, *sibling, rij, wid);

						ep.energy.intravdw += ij.energy; // Sum nonbonded energy.
					}
                }
            }
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

	EPTuple ForceFieldManager::EvaluateEnergy(const Particle& particle) const
	{
		return EvaluateInterEnergy(particle) + EvaluateIntraEnergy(particle);
	}

	EPTuple ForceFieldManager::EvaluateEnergy(const World& world) const
	{
		return EvaluateInterEnergy(world) + EvaluateIntraEnergy(world) + EvaluateTailEnergy(world);
	}
}