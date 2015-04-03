#pragma once

#include "../Particles/Particle.h"
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

			// Evaluate the Hamiltonian of the particle.
			inline double EvaluateHamiltonian(Particle& particle)
			{
				double h = 0;
				
				// Calculate energy with neighbors.
				auto& neighbors = particle.GetNeighbors();
				for(auto& neighbor : neighbors)
				{
					auto* particle2 = neighbor.GetParticle();

					auto* ff = _forcefields[particle.GetIdentifier()][particle2->GetIdentifier()];
					if(ff != nullptr)
						h += ff->Evaluate(particle, *particle2);
				}

				// Calculate energy of children.
				for(auto &child : particle.GetChildren())
					h += EvaluateHamiltonian(*child);

				// Calculate connectivity energy 
				for(auto &connectivity : particle.GetConnectivities())
					h+= connectivity->EvaluateHamiltonian(&particle);

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
