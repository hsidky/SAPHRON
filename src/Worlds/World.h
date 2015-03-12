#pragma once

#include "../Particles/Particle.h"
#include <memory>

namespace SAPHRON
{
	typedef std::shared_ptr<Particle> ParticlePtr;

	// Abstract base class representing the "World" in which particles live. A World object is
	// responsible for setting up the "box" and associated geometry, handling boundary conditions and
	// updating negihbor lists on particles.
	class World
	{
		private:

		protected:
			std::vector<ParticlePtr> _particles;

		public:
			// Draw a random particle from the world.
			virtual ParticlePtr DrawRandomParticle() = 0;

			// Update the neighbor list for all particles in the world.
			virtual void UpdateNeighborList() {}

			// Get number of particles in the world.
			int GetParticleCount()
			{
				return (int)_particles.size();
			}

			// Get a specific particle based on location.
			ParticlePtr GetParticle(int location)
			{
				return _particles[location];
			}

			//virtual int GetWorldParticleCount() = 0;

			virtual ~World (){}
	};
}
