#pragma once

#include "../Particles/Particle.h"
#include "../Rand.h"
#include "World.h"
#include <cmath>
#include <memory>
#include <vector>

namespace SAPHRON
{
	// A class describing a simple lattice world with periodic boundary conditions.
	class SimpleLatticeWorld : public World
	{
		private:
			int _xlength, _ylength, _zlength;
			std::vector<Particle*> _particles;
			Rand _rand;

		protected:
			// Visit children.
			virtual void VisitChildren(class Visitor &v) override
			{
				for(auto& particle : _particles)
					particle->AcceptVisitor(v);
			}

		public:
			SimpleLatticeWorld(int xlength, int ylength, int zlength, int seed = 1)
				: _xlength(xlength), _ylength(ylength), _zlength(zlength), _rand(seed)
			{
			}

			// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
			// initialized with the appropriate composition.
			void ConfigureParticles(const std::vector<Particle*> particles,
			                        const std::vector<double> fractions);
			

			// Draw a random particle from the world.
			inline virtual Particle* DrawRandomParticle() override
			{
				return _particles[_rand.int32() % this->_particles.size()];
			}

			// Draw random particles from the world and inserts them into particles. NOTE: this method 
			// clears the list before adding new elements.
			inline virtual void DrawRandomParticles(ParticleList& particles, 
											        unsigned int count = 1) override 
			{
				particles.resize(count);
				unsigned int n = this->_particles.size();
				for(unsigned int i = 0; i < count; i ++)
					particles[i] = _particles[_rand.int32() % n];
			}

			// Get number of high level particles in the world.
			virtual int GetParticleCount() const override
			{
				return (int)_particles.size();
			}

			// Get a particle by index.
			virtual Particle* SelectParticle(int location) override
			{
				return _particles[location];
			}

			// Get a particle by index (const).
			virtual const Particle*  SelectParticle(int location) const override
			{
				return _particles[location];
			}

			// Add particle.
			virtual void AddParticle(Particle*&& particle) override
			{
				_particles.push_back(std::move(particle));
			}

			// Remove a particle by index.
			virtual void RemoveParticle(int location) override
			{
				_particles.erase(_particles.begin() + location);
			}

			// Remove particle(s) based on a supplied filter.
			virtual void RemoveParticle(std::function<bool(Particle*)> filter) override
			{
				_particles.erase(std::remove_if(_particles.begin(), _particles.end(), filter), 
								_particles.end());
			}

			int GetLatticeSize()
			{
				return _xlength*_ylength*_zlength;
			}

			void ConfigureNeighborList();			

			~SimpleLatticeWorld()
			{
				for(auto it = _particles.begin(); it != _particles.end(); ++it)
					delete *it;

				_particles.clear();
			}
	};
}
