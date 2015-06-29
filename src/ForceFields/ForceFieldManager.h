#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include "../Ensembles/Ensemble.h"
#include "../Observers/Visitable.h"
#include "ForceField.h"
#include <math.h>
#include <map>
#include <iterator>

namespace SAPHRON
{
	typedef std::pair<int, int> SpeciesPair;
	typedef std::map<SpeciesPair, ForceField*> FFMap;

	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager : public Visitable
	{
	private:
		//non-bonded forcefields
		FFMap _nonbondedforcefields;

		//bonded forcefields
		FFMap _bondedforcefields;

		// Hold unique instances of non-bonded and bonded forcefield pointers.
		FFMap _uniquenbffs;
		FFMap _uniquebffs;

		// Evaluate non-bonded interactions of a particle including energy and virial pressure contribution.
		// Implementation follows Allan and Tildesley. See Forcefield.h. Tail corrections are also summed in.
		// Pressure tail contribution is added to isotropic pressure parts only!
		inline EPTuple EvaluateNonBonded(Particle& particle, const CompositionList& compositions, double volume)
		{
			double e = 0, pxx = 0, pxy = 0, pxz = 0, pyy = 0, pyz = 0, pzz = 0;

			// Calculate energy with neighbors.
			auto& neighbors = particle.GetNeighbors();
			#pragma omp parallel for reduction(+:e,pxx,pxy,pxz,pyy,pyz,pzz)
			for(size_t k = 0; k < neighbors.size(); ++k)
			{
				auto* neighbor = neighbors[k];
				auto* ff = _nonbondedforcefields[{particle.GetSpeciesID(),neighbor->GetSpeciesID()}];
				Position rij = particle.GetPositionRef() - neighbor->GetPositionRef();
				
				// Minimum image convention.
				World* world = particle.GetWorld();
				if(world != nullptr)
					world->ApplyMinimumImage(rij);

				if(ff != nullptr)
				{
					// Interaction containing energy and virial.
					auto ij = ff->Evaluate(particle, *neighbor, rij);
					e += ij.energy; // Sum nonbonded energy.
					
					pxx += ij.virial * rij.x * rij.x;
					pyy += ij.virial * rij.y * rij.y;
					pzz += ij.virial * rij.z * rij.z;
					pxy += ij.virial * rij.x * rij.y;
					pxz += ij.virial * rij.x * rij.z;
					pyz += ij.virial * rij.y * rij.z;
				}

				// Iterate children with neighbor's children.
				// TODO: needs cleanup to eliminate redundant inner child loop.
				// The subtlety arises in that the parent holds the neighbor list rather
				// than the child. We keep the final call for children below in case for some 
				// implementation they do - in which case it's taken care of.
				/*for(auto& child : particle.GetChildren())
				{
					for(auto& nchild : neighbor->GetChildren())
					{
						Position nrij = child->GetPosition() - neighbor->GetPosition();
						ff = _nonbondedforcefields[child->GetSpeciesID()][nchild->GetSpeciesID()];
						if(ff != nullptr)
						{
							// Interaction containing energy and virial.
							auto ij = ff->Evaluate(*child, *nchild, nrij);
							e += ij.energy; // Sum nonbonded energy.
						
							// Sum pressure terms. Average non-diagonal elements.
							// We are assuming it's symmetric.
							pxx -= ij.virial * nrij.x * rij.x;
							pyy -= ij.virial * nrij.y * rij.y;
							pzz -= ij.virial * nrij.z * rij.z;
							pxy -= ij.virial * 0.5*(nrij.x * rij.y + nrij.y * rij.x);
							pxz -= ij.virial * 0.5*(nrij.x * rij.z + nrij.z * rij.x);
							pyz -= ij.virial * 0.5*(nrij.y * rij.z + nrij.z * rij.y);
						}
					}

					// Sum in child energy and pressure tail corrections.
					if(!compositions.empty())
					{
						int i = 0; 
						for(auto& forcefield : _nonbondedforcefields[child->GetSpeciesID()])
						{
							if(forcefield != nullptr)
							{
								double rho = (compositions.at(i))/volume;
								e += 2.0*M_PI*rho*forcefield->EnergyTailCorrection();
								double pcorrect = 2.0*M_PI*rho*rho*forcefield->PressureTailCorrection();
								pxx -= pcorrect;
								pyy -= pcorrect;
								pzz -= pcorrect;
							}
							++i;
						}
					}
				}*/
			}
			
			EPTuple ep{e, 0, 0, 0, -pxx, -pxy, -pxz, -pyy, -pyz, -pzz, 0};				

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
						ep.energy.nonbonded += 2.0*2.0*M_PI*rho*forcefield->EnergyTailCorrection();
						ep.pressure.ptail = 2.0/3.0*2.0*M_PI*rho*forcefield->PressureTailCorrection();
					}
				}
			}
			
			for(auto& child : particle.GetChildren())
				ep += EvaluateNonBonded(*child, compositions, volume);	
			return ep;
		}

		inline double EvaluateBonded(Particle& particle)
		{
			double e = 0;

			// Calculate energy with bonded neighbors.
			auto& bondedneighbors = particle.GetBondedNeighbors();

			for(size_t k = 0; k < bondedneighbors.size(); ++k)
			{
				auto* bondedneighbor = bondedneighbors[k];
				auto* ff = _bondedforcefields[{particle.GetSpeciesID(),bondedneighbor->GetSpeciesID()}];
				Position rij = particle.GetPositionRef() - bondedneighbor->GetPositionRef();
				
				// Minimum image convention.
				World* world = particle.GetWorld();

				if(world != nullptr)
					world->ApplyMinimumImage(rij);

				if(ff != nullptr)
				{
					// Interaction containing energy and virial.
					auto ij = ff->Evaluate(particle, *bondedneighbor, rij);
					e += ij.energy; // Sum nonbonded energy.
				}
			}
			
			
			for(auto& child : particle.GetChildren())
				e += EvaluateBonded(*child)/2.0;	
			return e;			
		}

		inline double EvaluateConnectivity(Particle& particle)
		{
			double h = 0;

			// Calculate connectivity energy 
			for(auto &connectivity : particle.GetConnectivities())
				h+= connectivity->EvaluateEnergy(&particle);

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

		// Evaluate the energy and virial contribution of the entire world.
		inline EPTuple EvaluateHamiltonian(World& world)
		{
			EPTuple ep;

			for (int i = 0; i < world.GetParticleCount(); ++i)
			{
				auto* particle = world.SelectParticle(i);
				ep += EvaluateHamiltonian(*particle, world.GetComposition(), world.GetVolume());	
			}

			ep.energy.nonbonded *= 0.5;
			ep.energy.bonded *=0.5;
			ep.pressure *= 0.5;

			return ep;
		}

		// Evaluate the energy and virial contribution of a list of particles.
		inline EPTuple EvaluateHamiltonian(const ParticleList& particles, const CompositionList& compositions, double volume)
		{
			EPTuple ep;
			for(auto& particle : particles)
				ep += EvaluateHamiltonian(*particle, compositions, volume);

			return ep;
		}

		// Evaluate the energy and virial contribution of the particle.
		inline EPTuple EvaluateHamiltonian(Particle& particle, const CompositionList& compositions, double volume)
		{
			EPTuple ep = EvaluateNonBonded(particle, compositions, volume);
			ep.energy.bonded += EvaluateBonded(particle);
			ep.energy.connectivity = EvaluateConnectivity(particle);

			// Divide virial by volume to get pressure. 
			ep.pressure /= volume;
			return ep;
		}

		// Accept a visitor.
		virtual void AcceptVisitor(class Visitor &v) override
		{
			v.Visit(this);
		}

		// Iterators. TODO: Fix iterator such that only unique instances of 
		// forcefields get returned. Also add _uniquebffs
		iterator begin() { return _uniquenbffs.begin(); }
		iterator end() { return _uniquenbffs.end(); }
	};
}
