#pragma once

#include "../Particles/Particle.h"
#include "../Rand.h"
#include "World.h"
#include <cmath>
#include <memory>
#include <vector>

namespace SAPHRON
{
	// A class describing a simple world with periodic boundary conditions.
	class SimpleWorld : public World
	{
		private:
			double _xlength, _ylength, _zlength;
			std::vector<Particle*> _particles;
			Rand _rand;

			// Neighbor list cutoff radius.
			double _ncut;

			// Composition of the system.
			CompositionList _composition;

			void AddParticleComposition(Particle* particle);
			void RemoveParticleComposition(Particle* particle);
			void ModifyParticleComposition(const ParticleEvent& pEvent);
			void BuildCompositionList();

		protected:
			// Visit children.
			virtual void VisitChildren(class Visitor &v) override
			{
				for(auto& particle : _particles)
					particle->AcceptVisitor(v);
			}

		public:
			SimpleWorld(double xlength, double ylength, double zlength, double ncut, int seed = 1)
				: _xlength(xlength), _ylength(ylength), _zlength(zlength), _rand(seed), _ncut(ncut)
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

			// Get system composition.
			virtual const CompositionList& GetComposition() const
			{
				return _composition;
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

			// Add particle. TODO: CAREFUL with the move semantics. 
			// Need to visit this further.
			virtual void AddParticle(Particle&& particle) override
			{
				particle.AddObserver(this);
				AddParticleComposition(&particle);
				_particles.push_back(std::move(&particle));
			}

			// Add particle. Re-connect neighbors if they exist.
			virtual void AddParticle(Particle* particle) override
			{
				particle->AddObserver(this);
				for(auto& neighbor : particle->GetNeighbors())
					neighbor->AddNeighbor(particle);
				AddParticleComposition(particle);
				_particles.push_back(particle);
			}

			// Remove a particle by index.
			virtual void RemoveParticle(int location) override
			{
				Particle* p = _particles[location];
				p->RemoveFromNeighbors();
				p->RemoveObserver(this);
				RemoveParticleComposition(p);
				_particles.erase(_particles.begin() + location);
			}

			// Remove particle by value.
			virtual void RemoveParticle(Particle* particle) override 
			{
				auto it = std::find(_particles.begin(), _particles.end(), particle);
				if(it != _particles.end())
				{
					particle->RemoveFromNeighbors();
					particle->RemoveObserver(this);
					RemoveParticleComposition(particle);
					_particles.erase(it);
				}
			}

			// Remove particle(s) based on a supplied filter.
			// TODO: NEEDS UPDATING TO CARRY OUT IMPORTANT OPERATIONS. SEE ABOVE!
			virtual void RemoveParticle(std::function<bool(Particle*)> filter) override
			{
				_particles.erase(std::remove_if(_particles.begin(), _particles.end(), filter), 
								_particles.end());
			}

			// Particle observer to update world composition.
			virtual void ParticleUpdate(const ParticleEvent& pEvent) override
			{
				if(!pEvent.species)
					return;
				
				ModifyParticleComposition(pEvent);
			}

			// Apply periodic boundary conditions to particle position.
			virtual void ApplyPeriodicBoundaries(Particle* particle)
			{
				auto& prevP = particle->GetPositionRef();
				particle->SetPosition(prevP.x - _xlength*anint(prevP.x/_xlength),
									  prevP.y - _ylength*anint(prevP.y/_ylength),
									  prevP.z - _zlength*anint(prevP.z/_zlength));
			}


			// Sets the neighbor list cutoff radius.
			virtual void SetNeighborRadius(double ncut) override
			{
				_ncut = ncut;
			}

			// Gets the neighbor list cutoff radius.
			virtual double GetNeighborRadius() override
			{
				return _ncut; 
			}

			int GetLatticeSize()
			{
				return _xlength*_ylength*_zlength;
			}

			// Update the neighbor list for all particles in the world.
			virtual void UpdateNeighborList() override;		

			// Get system volume.
			virtual double GetVolume() override
			{
				return _xlength*_ylength*_zlength;
			}	

			~SimpleWorld()
			{
				for(auto it = _particles.begin(); it != _particles.end(); ++it)
					delete *it;

				_particles.clear();
			}
	};
}
