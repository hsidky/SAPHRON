#pragma once

#include "../Particles/Particle.h"
#include "ForceField.h"
#include <iostream>
#include <map>
#include <utility>

namespace SAPHRON
{
	typedef std::pair<int, int> ParticlePair;
	typedef std::map<ParticlePair, ForceField*>::iterator ForceFieldIterator;

	// Class responsible for managing forcefields and evaluating energies of particles.
	class ForceFieldManager
	{
		private:
			std::map<ParticlePair, ForceField*> _forcefields;

		public:

			// Adds a forcefield to the manager.
			void AddForceField(std::string p1type, std::string p2type, ForceField& ff)
			{
				auto map = Particle::GetIdentityMap();
				auto p1 = map.find(p1type);
				auto p2 = map.find(p2type);
				if(p1 == map.end() || p2 == map.end())
				{
					std::cerr << "ERROR: Uknown particle types. "
					          << "Make sure they have been registered with the identity map."
					          << std::endl;
					exit(-1);
				}

				AddForceField(p1->second, p2->second, ff);
			}

			// Adds a forcefield to the manager.
			void AddForceField(int p1type, int p2type, ForceField& ff)
			{
				_forcefields.insert({{p1type, p2type}, &ff});
			}

			// Removes a forcefield from the manager.
			void RemoveForceField(std::string p1type, std::string p2type)
			{
				auto map = Particle::GetIdentityMap();
				auto p1 = map.find(p1type);
				auto p2 = map.find(p2type);
				if(p1 == map.end() || p2 == map.end())
				{
					std::cerr << "ERROR: Uknown particle types. "
					          << "Make sure they have been registered with the identity map."
					          << std::endl;
					exit(-1);
				}

				RemoveForceField(p1->second, p2->second);
			}

			// Removes a forcefield from the manager.
			void RemoveForceField(int p1type, int p2type)
			{
				_forcefields.erase(_forcefields.find({p1type, p2type}));
			}

			// Evaluate the Hamiltonian of the particle.
			double EvaluateHamiltonian(Particle& particle)
			{
				double h = 0;
				auto& neighbors = particle.GetNeighborList();
				for(auto& neighbor : neighbors)
				{
					auto* particle2 = neighbor.GetParticle();
					auto& search = _forcefields.find({particle.GetIdentifier(),
					                                  particle2->GetIdentifier()});
					if(search != _forcefields.end())
					{
						auto ff = search->second;
						h += ff->Evaluate(particle, *particle2);
					}
				}

				for(auto &child : particle.GetChildren())
					h += EvaluateHamiltonian(*child);

				return h;
			}
	};
}
