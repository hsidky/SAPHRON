#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include "../Observers/Visitable.h"
#include "../JSON/Serializable.h"
#include "ForceField.h"
#include "vecmap.h"
#include <math.h>
#include <map>
#include <iterator>
#include <limits>

namespace SAPHRON
{
	typedef std::pair<int, int> SpeciesPair;
	typedef vecmap<SpeciesPair, ForceField*> FFMap;

	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager : public Visitable, public Serializable
	{
	private:
		//non-bonded forcefields
		FFMap _nonbondedforcefields;

		//bonded forcefields
		FFMap _bondedforcefields;

		//electrostatic forcefield
		FFMap _electrostaticforcefield;

		// Hold unique instances of non-bonded and bonded forcefield pointers.
		FFMap _uniquenbffs;
		FFMap _uniquebffs;
		FFMap _uniqueeffs;

		// Default cutoff.
		double _rcdefault = 10e10;

		// Evaluate intermolecular interactions of a particle including energy and virial pressure contribution.
		// Implementation follows Allen and Tildesley. See Forcefield.h. Tail corrections are also summed in.
		// Pressure tail contribution is added to isotropic pressure parts only!
		inline EPTuple EvaluateInter(const Particle& particle, const CompositionList& compositions, double volume) const
		{
			double intere = 0, electroe = 0, pxx = 0, pxy = 0, pxz = 0, pyy = 0, pyz = 0, pzz = 0;

			// Calculate energy with neighbors.
			World* world = particle.GetWorld();
			auto& neighbors = particle.GetNeighbors();
			double rcut = (world == nullptr) ? _rcdefault : world->GetCutoffRadius();
			if(!particle.HasChildren())
			{
				#pragma omp parallel for reduction(+:intere,electroe,pxx,pxy,pxz,pyy,pyz,pzz)
				for(size_t k = 0; k < neighbors.size(); ++k)
				{
					auto* neighbor = neighbors[k];

					Position rij = particle.GetPosition() - neighbor->GetPosition();
												
					if(world != nullptr)
						world->ApplyMinimumImage(rij);

					// skip if exceeds cutoff.
					if(arma::norm(rij) > rcut)
						continue;

					// If particle has parent, compute vector between parent molecule(s).
					Position rab = rij;
					if(particle.HasParent() && neighbor->HasParent())
					{
						rab = particle.GetParent()->GetPosition() - neighbor->GetParent()->GetPosition();
						if(world != nullptr)
							world->ApplyMinimumImage(rab);

					}
					else if(neighbor->HasParent() && !particle.HasParent()) 
					{
						rab = particle.GetPosition() - neighbor->GetParent()->GetPosition();
						if(world != nullptr)
							world->ApplyMinimumImage(rab);
					}
					else if(!neighbor->HasParent() && particle.HasParent()) {
						rab = particle.GetParent()->GetPosition() - neighbor->GetPosition();
						if(world != nullptr)
							world->ApplyMinimumImage(rab);
					}

					auto it = _nonbondedforcefields.find({particle.GetSpeciesID(),neighbor->GetSpeciesID()});
					auto it2 = _electrostaticforcefield.find({particle.GetSpeciesID(),neighbor->GetSpeciesID()});

					Interaction interij, electroij;

					// Interaction containing energy and virial.
					if(it != _nonbondedforcefields.end())
					{
						auto* ff = it->second;
						interij = ff->Evaluate(particle, *neighbor, rij, rcut);
					}

					//Electrostatics containing energy and virial
					if(it2 != _electrostaticforcefield.end())
					{
						auto* eff = it2->second;
						electroij = eff->Evaluate(particle, *neighbor, rij, rcut);
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
			
			EPTuple ep{intere, 0, electroe, 0, 0, 0, 0, -pxx, -pxy, -pxz, -pyy, -pyz, -pzz, 0};				

			// Sum in energy and pressure tail corrections.
			// Note: we multiply the tail correction expressions by 2.0 because they are on a per-particle
			// basis, but we divide non-bonded energy contributions by 2.0 to avoid double counting. So 
			// this is a correction. Also the pressure tail correction does not contain another "rho" term. 
			// This is because it is summed over all particles and divided by volume. Effectively another "rho".
			if(!compositions.empty())
			{
				for(auto& it : _nonbondedforcefields)
				{
					const auto& key = it.first;
					if(key.first == particle.GetSpeciesID())
					{
						auto* forcefield = it.second;
						double N = compositions.at(key.second);
						double rho = N/volume;
						ep.energy.intervdw += 2.0*2.0*M_PI*rho*forcefield->EnergyTailCorrection(rcut);
						ep.pressure.ptail = 2.0/3.0*2.0*M_PI*rho*forcefield->PressureTailCorrection(rcut);
					}
				}
			}
			
			for(auto& child : particle.GetChildren())
				ep += EvaluateInter(*child, compositions, volume);	
			return ep;
		}

		// Evaluate intramolecular interactions of a particle.
		inline Energy EvaluateIntra(const Particle& particle, const CompositionList& compositions, double volume) const
		{
			EPTuple ep(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

			World* world = particle.GetWorld();
			double rcut = (world == nullptr) ? _rcdefault : world->GetCutoffRadius();

    		// Go to particle parent and evaluate non-bonded interactions with siblings.
            if(particle.HasParent())
            { 
                for(auto& sibling : *particle.GetParent())
                {
                    if(!particle.IsBondedNeighbor(sibling) && sibling != &particle)
                    {
                    	
                    	Position rij = particle.GetPosition() - sibling->GetPosition();
    					
    					if(world != nullptr)
							world->ApplyMinimumImage(rij);

						auto it = _nonbondedforcefields.find({particle.GetSpeciesID(),sibling->GetSpeciesID()});
						auto it2 = _electrostaticforcefield.find({particle.GetSpeciesID(),sibling->GetSpeciesID()});

						//Electrostatics containing energy and virial
						if(it2 != _electrostaticforcefield.end())
						{
							auto* eff = it2->second;
							auto ij = eff->Evaluate(particle, *sibling, rij, rcut);
							ep.energy.intraelectrostatic+=ij.energy;
						}		

						if(it != _nonbondedforcefields.end())
						{
							auto* ff = it->second;
							auto ij = ff->Evaluate(particle, *sibling, rij, rcut);

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
						world->ApplyMinimumImage(rij);

					auto ij = ff->Evaluate(particle, *bondedneighbor, rij, rcut);
					ep.energy.bonded += ij.energy; // Sum bonded energy.
					
				}
			}
			
			for(auto& child : particle)
				ep.energy += EvaluateIntra(*child, compositions, volume)/2.0;	
			return ep.energy;
		}

		inline double EvaluateConnectivity(const Particle& particle) const
		{
			double h = 0;

			// Calculate connectivity energy 
			for(auto &connectivity : particle.GetConnectivities())
				h+= connectivity->EvaluateEnergy(particle);

			// Calculate energy of children.
			for(auto &child : particle.GetChildren())
				h += EvaluateConnectivity(*child);

			return h;
		}

	public:
		typedef FFMap::iterator iterator;
		typedef FFMap::const_iterator const_iterator;
		
		ForceFieldManager() : _nonbondedforcefields(), _bondedforcefields() {}

		// Adds a non-bonded forcefield to the manager.
		void AddNonBondedForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a non-bonded forcefield to the manager.
		void AddNonBondedForceField(int p1type, int p2type, ForceField& ff);

		// Removes a non-bonded forcefield from the manager.
		void RemoveNonBondedForceField(std::string p1type, std::string p2type);

		// Removes a non-bonded forcefield from the manager.
		void RemoveNonBondedForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int NonBondedForceFieldCount();

		// Adds a bonded forcefield to the manager.
		void AddBondedForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a bonded forcefield to the manager.
		void AddBondedForceField(int p1type, int p2type, ForceField& ff);

		// Removes a bonded forcefield from the manager.
		void RemoveBondedForceField(std::string p1type, std::string p2type);

		// Removes a bonded forcefield from the manager.
		void RemoveBondedForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int BondedForceFieldCount();

		// Adds a bonded forcefield to the manager.
		void AddElectrostaticForceField(std::string p1type, std::string p2type, ForceField& ff);
		
		// Adds a Electrostatic forcefield to the manager.
		void AddElectrostaticForceField(int p1type, int p2type, ForceField& ff);

		// Removes a Electrostatic forcefield from the manager.
		void RemoveElectrostaticForceField(std::string p1type, std::string p2type);

		// Removes a Electrostatic forcefield from the manager.
		void RemoveElectrostaticForceField(int p1type, int p2type);

		// Get the number of registered forcefields.
		int ElectrostaticForceFieldCount();

		// Evaluate the energy and virial contribution of the entire world.
		inline EPTuple EvaluateHamiltonian(World& world) const
		{
			EPTuple ep;

			for (int i = 0; i < world.GetParticleCount(); ++i)
			{
				auto* particle = world.SelectParticle(i);
				ep += EvaluateHamiltonian(*particle, world.GetComposition(), world.GetVolume());	
			}

			//Correct for double counting
			//Intra and bonded energies already corrected for in EvaluateIntra function
			ep.energy.intervdw *= 0.5;
			ep.energy.interelectrostatic *=0.5;
			ep.pressure *= 0.5;

			return ep;
		}

		// Evaluate the energy and virial contribution of a list of particles.
		inline EPTuple EvaluateHamiltonian(const ParticleList& particles, 
										   const CompositionList& compositions, 
										   double volume) const
		{
			EPTuple ep;
			for(auto& particle : particles)
				ep += EvaluateHamiltonian(*particle, compositions, volume);

			return ep;
		}

		// Evaluate the energy and virial contribution of the particle.
		inline EPTuple EvaluateHamiltonian(const Particle& particle, 
										   const CompositionList& compositions, 
										   double volume) const
		{
			EPTuple ep = EvaluateInter(particle, compositions, volume);
			ep.energy += EvaluateIntra(particle, compositions, volume);
			ep.energy.connectivity = EvaluateConnectivity(particle);

			// Divide virial by volume to get pressure. 
			ep.pressure /= volume;
			return ep;
		}

		// Accept a visitor.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
		}

		// Serialize forcefield manager.
		virtual void Serialize(Json::Value& json) const override
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

			if(_uniqueeffs.size() != 0)
			{
				auto& electro = json["forcefields"]["electrostatic"];
				for(auto& ff : _uniqueeffs)
				{
					auto& pair = ff.first;
					auto& last = electro[electro.size()];
					ff.second->Serialize(last);
					last["species"][0] = species[pair.first];
					last["species"][1] = species[pair.second];
				}
			}
		}

		// Get (unique) non-bonded forcefields.
		const FFMap& GetNonBondedForceFields() const { return _uniquenbffs;	}

		// Get (unique) bonded forcefields.
		const FFMap& GetBondedForceFields() const { return _uniquebffs;	}

		// Get default cutoff radius.
		double GetDefaultCutoff() const { return _rcdefault; }

		// Set default cutoff radius.
		void SetDefaultCutoff(double rc) { _rcdefault = rc; }
	};
}
