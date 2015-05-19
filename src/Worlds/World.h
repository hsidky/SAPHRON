#pragma once

#include "../Particles/Particle.h"
#include "../Visitors/Visitable.h"
#include <memory>
#include <functional>

namespace SAPHRON
{
	// Public interface representing the "World" in which particles live. A World object is
	// responsible for setting up the "box" and associated geometry, handling boundary conditions and
	// updating negihbor lists on particles.
	class World : public Visitable
	{
		protected:
			// Visit children.
			virtual void VisitChildren(class Visitor &v) = 0;

		public:
			// Draw a random particle from the world.
			virtual Particle* DrawRandomParticle() = 0;

			// Draw random particles from the world and adds them to the list provided. 
			virtual void DrawRandomParticles(ParticleList& particles, unsigned int count = 1) = 0;

			// Update the neighbor list for all particles in the world.
			virtual void UpdateNeighborList() = 0;

			// Get number of high level particles in the world.
			virtual int GetParticleCount() const = 0;

			// Get a specific particle based on location.
			virtual Particle* SelectParticle(int location) = 0;

			// Get a specific particle based on location (const).
			virtual const Particle* SelectParticle(int location) const = 0;
			
			// Add a particle. 
			virtual void AddParticle(Particle*&& particle) = 0;

			// Remove a specific particle based on location.
			virtual void RemoveParticle(int location) = 0;

			// Remove particle(s) based on a supplied filter.
			virtual void RemoveParticle(std::function<bool(Particle*)> filter) = 0;

			//virtual int GetWorldParticleCount() = 0;

			// Accept a visitor.
			virtual void AcceptVisitor(class Visitor &v) override
			{
				v.Visit(this);
				VisitChildren(v);
			}


			virtual ~World (){}
	};
}
