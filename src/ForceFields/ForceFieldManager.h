#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"
#include "../Ensembles/Ensemble.h"
#include "ForceField.h"
#include <math.h>

namespace SAPHRON
{
	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager
	{
		private:
			std::vector<std::vector<ForceField*> > _forcefields;

			// Resize Forcefield vector.
			void ResizeFF(int n);

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
					auto* ff = _forcefields[particle.GetSpeciesID()][neighbor->GetSpeciesID()];
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
						
						pxx -= ij.virial * rij.x * rij.x;
						pyy -= ij.virial * rij.y * rij.y;
						pzz -= ij.virial * rij.z * rij.z;
						pxy -= ij.virial * rij.x * rij.y;
						pxz -= ij.virial * rij.x * rij.z;
						pyz -= ij.virial * rij.y * rij.z;
					}

					// Iterate children with neighbor's children.
					// TODO: needs cleanup to eliminate redundant inner child loop.
					// The subtlety arises in that the parent holds the neighbor list rather
					// than the child. We keep the final call for children below in case for some 
					// implementation they do - in which case it's taken care of.
					for(auto& child : particle.GetChildren())
					{
						for(auto& nchild : neighbor->GetChildren())
						{
							Position nrij = child->GetPosition() - neighbor->GetPosition();
							ff = _forcefields[child->GetSpeciesID()][nchild->GetSpeciesID()];
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
							for(auto& forcefield : _forcefields[child->GetSpeciesID()])
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
					}
				}
				
				EPTuple ep{e, 0, 0, pxx, pxy, pxz, pyy, pyz, pzz};				

				// Sum in energy and pressure tail corrections.
				// Sum in child energy and pressure tail corrections.
				if(!compositions.empty())
				{
					int i = 0; 
					for(auto& forcefield : _forcefields[particle.GetSpeciesID()])
					{
						if(forcefield != nullptr)
						{
							double rho = (compositions.at(i))/volume;
							ep.energy.nonbonded += 2.0*M_PI*rho*forcefield->EnergyTailCorrection();
							double pcorrect = 2.0*M_PI*rho*rho*forcefield->PressureTailCorrection();
							ep.pressure.pxx -= pcorrect;
							ep.pressure.pyy -= pcorrect;
							ep.pressure.pyz -= pcorrect;
						}
						++i;
					}
				}
				
				for(auto& child : particle.GetChildren())
					ep += EvaluateNonBonded(*child, compositions, volume);	
				return ep;
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

			ForceFieldManager() : _forcefields(1,std::vector<ForceField*>(1, nullptr)) {}

			// Adds a forcefield to the manager.
			void AddForceField(std::string p1type, std::string p2type, ForceField& ff);
			
			// Adds a forcefield to the manager.
			void AddForceField(int p1type, int p2type, ForceField& ff);

			// Removes a forcefield from the manager.
			void RemoveForceField(std::string p1type, std::string p2type);

			// Removes a forcefield from the manager.
			void RemoveForceField(int p1type, int p2type);

			// Evaluate the energy and virial contribution of the entire world.
			inline EPTuple EvaluateHamiltonian(World& world)
			{
				EPTuple ep;

				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto* particle = world.SelectParticle(i);
					ep += EvaluateHamiltonian(*particle, world.GetComposition(), world.GetVolume());	
					ep.energy.connectivity += EvaluateConnectivity(*particle);
				}

				ep.energy.nonbonded *= 0.5;
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
				ep.energy.connectivity = EvaluateConnectivity(particle);

				// Divide virial by volume to get pressure. 
				ep.pressure /= volume;
				return ep;
			}
	};
}
