#pragma once

#include "../Particles/Particle.h"
#include "../Visitors/Visitable.h"
#include <memory>
#include <functional>
#include <map>

namespace SAPHRON
{
	typedef std::map<int, int> CompositionList; 

	// Public interface representing the "World" in which particles live. A World object is
	// responsible for setting up the "box" and associated geometry, handling boundary conditions and
	// updating negihbor lists on particles. World implements ParticleObserver so it can "listen in" 
	// on it's particles if it needs it. Note: it is the World implementation's responsibility to register 
	// itself with particles if it needs it, and to remove this when a particle is removed.
	class World : public Visitable, public ParticleObserver
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
			virtual void AddParticle(Particle&& particle) = 0;

			// Add a particle.
			virtual void AddParticle(Particle* particle) = 0;

			// Remove a specific particle based on location.
			virtual void RemoveParticle(int location) = 0;

			virtual void RemoveParticle(Particle* p) = 0;

			// Remove particle(s) based on a supplied filter.
			virtual void RemoveParticle(std::function<bool(Particle*)> filter) = 0;

			// Get neighbor list cutoff radius.
			virtual double GetNeighborRadius() = 0;

			// Set neighbor list radius cutoff.
			virtual void SetNeighborRadius(double ncut) = 0;

			// Set skin thickness for neighbor list re-generation.
			virtual void SetSkinThickness(double x) = 0;

			// Get skin thickness for neighbor list re-generation.
			virtual double GetSkinThickness() = 0;

			// Applies minimum image convention to distances. 
			virtual void ApplyMinimumImage(Position& position) = 0;

			// Applies periodic boundaries to positions.
			virtual void ApplyPeriodicBoundaries(Position& position) = 0;

			// Accept a visitor.
			virtual void AcceptVisitor(class Visitor &v) override
			{
				v.Visit(this);
				VisitChildren(v);
			}

			// Particle observer.
			virtual void ParticleUpdate(const ParticleEvent& pEvent) override = 0;

			// Get system composition.
			virtual const CompositionList& GetComposition() const = 0;

			// Get system volume.
			virtual double GetVolume() = 0;

			virtual ~World (){}
	};
}
