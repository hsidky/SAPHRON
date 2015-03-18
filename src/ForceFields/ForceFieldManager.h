#pragma once

#include "../Particles/Particle.h"
#include "ForceField.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace SAPHRON
{
	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager
	{
		private:
			std::vector<std::vector<ForceField*> > _forcefields;

			void ResizeFF(int n)
			{
				_forcefields.resize(n);
				for(int i = 0; i < n; i++)
					_forcefields[i].resize(n, nullptr);
			}

		public:

			ForceFieldManager() : _forcefields(1,std::vector<ForceField*>(1, nullptr)) {}

			// Adds a forcefield to the manager.
			void AddForceField(std::string p1type, std::string p2type, ForceField& ff)
			{
				auto list = Particle::GetIdentityList();
				auto p1 = std::find(list.begin(), list.end(), p1type);
				auto p2 = std::find(list.begin(), list.end(), p2type);

				if(p1 == list.end() || p2 == list.end())
				{
					std::cerr << "ERROR: Uknown particle types. "
					          << "Make sure they have been registered with the identity map."
					          << std::endl;
					exit(-1);
				}

				AddForceField(p1-list.begin(), p2-list.begin(), ff);
			}

			// Adds a forcefield to the manager.
			void AddForceField(int p1type, int p2type, ForceField& ff)
			{
				if((int)_forcefields.size()-1 < std::max(p1type,p2type))
					ResizeFF(std::max(p1type,p2type)+1);

				(_forcefields.at(p1type)).at(p2type) = &ff;
				(_forcefields.at(p2type)).at(p1type) = &ff;
			}

			// Removes a forcefield from the manager.
			void RemoveForceField(std::string p1type, std::string p2type)
			{
				auto list = Particle::GetIdentityList();
				auto p1 = std::find(list.begin(), list.end(), p1type);
				auto p2 = std::find(list.begin(), list.end(), p2type);
				if(p1 == list.end() || p2 == list.end())
				{
					std::cerr << "ERROR: Uknown particle types. "
					          << "Make sure they have been registered with the identity map."
					          << std::endl;
					exit(-1);
				}

				RemoveForceField(p1-list.begin(), p2-list.begin());
			}

			// Removes a forcefield from the manager.
			void RemoveForceField(int p1type, int p2type)
			{
				(_forcefields.at(p1type)).at(p2type) = nullptr;
				(_forcefields.at(p2type)).at(p1type) = nullptr;
			}

			// Evaluate the Hamiltonian of the particle.
			inline double EvaluateHamiltonian(Particle& particle)
			{
				double h = 0;
				auto& neighbors = particle.GetNeighborList();
				for(auto& neighbor : neighbors)
				{
					auto* particle2 = neighbor.GetParticle();

					auto* ff = _forcefields[particle.GetIdentifier()][particle2->GetIdentifier()];
					if(ff != nullptr)
						h += ff->Evaluate(particle, *particle2);
				}

				for(auto &child : particle.GetChildren())
					h += EvaluateHamiltonian(*child);

				return h;
			}
	};
}
