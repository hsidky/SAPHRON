#pragma once

#include "../Particles/Particle.h"
#include "ForceField.h"
#include <map>
#include <utility>

namespace SAPHRON
{
	typedef std::pair<std::string, std::string> ParticlePair;
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
				_forcefields.insert({{p1type, p2type}, &ff});
			}

			// Removes a forcefield from the manager.
			void RemoveForceField(std::string p1type, std::string p2type)
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
					auto particle2 = neighbor.GetParticle();
					auto search = _forcefields.find(
					        {particle.GetIdentifier(), particle2->GetIdentifier()});
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
