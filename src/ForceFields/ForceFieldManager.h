#pragma once

#include "../Particles/Particle.h"
#include "../Properties/Energy.h"
#include "../Worlds/World.h"
#include "ForceField.h"

namespace SAPHRON
{
	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager
	{
		private:
			std::vector<std::vector<ForceField*> > _forcefields;

			// Resize Forcefield vector.
			void ResizeFF(int n);

			// Evaluate non-bonded interactions of a particle.
			inline double EvaluateNonBondedEnergy(Particle& particle)
			{
				double h = 0;
				
				// Calculate energy with neighbors.
				auto& neighbors = particle.GetNeighbors();
				for(auto& neighbor : neighbors)
				{
					auto* ff = _forcefields[particle.GetSpeciesID()][neighbor->GetSpeciesID()];
					if(ff != nullptr)
						h += ff->Evaluate(particle, *neighbor);
				}

				// Calculate energy of children.
				for(auto &child : particle.GetChildren())
					h += EvaluateNonBondedEnergy(*child);

				return h;
			}

			inline double EvaluateConnectivityEnergy(Particle& particle)
			{
				double h = 0;

				// Calculate connectivity energy 
				for(auto &connectivity : particle.GetConnectivities())
					h+= connectivity->EvaluateEnergy(&particle);

				// Calculate energy of children.
				for(auto &child : particle.GetChildren())
					h += EvaluateConnectivityEnergy(*child);

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

			// Evaluate the Hamiltonian of the entire world.
			inline Energy EvaluateHamiltonian(World& world)
			{
				Energy e(0, 0);

				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto* particle = world.SelectParticle(i);
					
					e.nonbonded += EvaluateNonBondedEnergy(*particle);
					e.connectivity += EvaluateConnectivityEnergy(*particle);
				}

				e.nonbonded *= 0.5;
				return e;
			}

			// Evaluate the Hamiltonian of the particle.
			inline Energy EvaluateHamiltonian(Particle& particle)
			{
				Energy e(0, 0);
					
				e.nonbonded = EvaluateNonBondedEnergy(particle);
				e.connectivity = EvaluateConnectivityEnergy(particle);
				return e;
			}

			// Evaluate the Hamiltonian of a list of particles.
			inline Energy EvaluateHamiltonian(const ParticleList& particles)
			{
				Energy e(0,0);
				for(auto& particle : particles)
					e += EvaluateHamiltonian(*particle);

				return e;
			}
	};
}
