#pragma once

#include "../Particles/Particle.h"
#include <memory>

namespace SAPHRON
{
	// Public interface representing the "World" in which particles live. A World object is
	// responsible for setting up the "box" and associated geometry, handling boundary conditions and
	// updating negihbor lists on particles.
	class World
	{

		public:
			// Draw a random particle from the world.
			virtual Particle* DrawRandomParticle() = 0;

			// Update the neighbor list for all particles in the world.
			virtual void UpdateNeighborList() {}

			// Get number of high level particles in the world.
			virtual int GetParticleCount() = 0;
			
			// Get a specific particle based on location.
			virtual Particle* GetParticle(int location) = 0;

			//virtual int GetWorldParticleCount() = 0;

			virtual ~World (){}
	};
}
