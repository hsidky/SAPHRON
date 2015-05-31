#pragma once

#include "../Particles/Particle.h"
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

			// Evaluate the only two body interactions of a particle.
			inline double EvaluateTwoBodyHamiltonian(Particle& particle)
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
					h += EvaluateTwoBodyHamiltonian(*child);

				return h;
			}

			inline double EvaluateConnectivityHamiltonian(Particle& particle)
			{
				double h = 0;

				// Calculate connectivity energy 
				for(auto &connectivity : particle.GetConnectivities())
					h+= connectivity->EvaluateEnergy(&particle);

				// Calculate energy of children.
				for(auto &child : particle.GetChildren())
					h += EvaluateConnectivityHamiltonian(*child);

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
			inline double EvaluateHamiltonian(World& world)
			{
				// Two body interactions.
				double h1 = 0;

				// Connectivity.
				double h2 = 0;

				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto* particle = world.SelectParticle(i);
					
					h1 += EvaluateTwoBodyHamiltonian(*particle);
					h2 += EvaluateConnectivityHamiltonian(*particle);
				}

				return 0.5*h1 + h2;
			}

			// Evaluate the Hamiltonian of the particle.
			inline double EvaluateHamiltonian(Particle& particle)
			{
				double h = 0;
				
				h+= EvaluateTwoBodyHamiltonian(particle);
				h+= EvaluateConnectivityHamiltonian(particle);

				return h;
			}

			// Evaluate the Hamiltonian of a list of particles.
			inline double EvaluateHamiltonian(const ParticleList& particles)
			{
				double h = 0.0; 
				for(auto& particle : particles)
					h += EvaluateHamiltonian(*particle);

				return h;
			}
	};
}
