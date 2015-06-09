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
			double _ncut, _ncutsq;

			// Skin thickness.
			double _rskin, _rskinsq; 

			// Composition of the system.
			CompositionList _composition;

			void AddParticleComposition(Particle* particle);
			void RemoveParticleComposition(Particle* particle);
			void ModifyParticleComposition(const ParticleEvent& pEvent);

			// Perform all the appropriate configuration for a new particle.
			inline void ConfigureParticle(Particle* particle)
			{
				// Add this world as an observer.
				particle->AddObserver(this);

				// Associate this world with particle.
				particle->SetWorld(this);

				// If particle has preexisiting neighbors, add it to them 
				// to maintain symmetry. 
				for(auto& neighbor : particle->GetNeighbors())
					neighbor->AddNeighbor(particle);

				// Add particle to the composition list.
				AddParticleComposition(particle);

				// Set particle checkpoint for neighbor lists.
				particle->SetCheckpoint();
			}

		protected:
			// Visit children.
			virtual void VisitChildren(class Visitor &v) override
			{
				for(auto& particle : _particles)
					particle->AcceptVisitor(v);
			}

		public:
			SimpleWorld(double xlength, double ylength, double zlength, double ncut, int seed = 1) : 
			_xlength(xlength), _ylength(ylength), _zlength(zlength), _particles(0), 
			_rand(seed), _ncut(ncut), _ncutsq(ncut*ncut), _rskin(1.0), _rskinsq(1.0)
			{
			}

			// Packs a SimpleWorld with the given particle blueprints and 
			// compositions to with "count" total particles and a specified density.
			void PackWorld(const std::vector<Particle*> particles,
			                      		  const std::vector<double> fractions, 
			                      		  int count, double density);

			// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
			// initialized with the appropriate composition.  If max is set, it will stop at that number.
			void ConfigureParticles(const std::vector<Particle*> particles,
			                        const std::vector<double> fractions, int max = 0);
			

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
			virtual const CompositionList& GetComposition() const override
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
				_particles.push_back(std::move(&particle));
				ConfigureParticle(_particles.back()); // Do this after since we are moving.
			}

			// Add particle. Re-connect neighbors if they exist.
			virtual void AddParticle(Particle* particle) override
			{
				ConfigureParticle(particle);
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
					particle->SetWorld(nullptr);
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
			inline virtual void ParticleUpdate(const ParticleEvent& pEvent) override
			{
				if(pEvent.species)
					ModifyParticleComposition(pEvent);

				/*if(pEvent.position)
				{
					// Check if we need to perform a neighbor list update.
					auto* p = pEvent.GetParticle();
					auto dist = p->GetCheckpointDist();
					ApplyMinimumImage(dist);
					if(dist.norm2() > _rskinsq/4.0)	
						UpdateNeighborList();				
				}*/

			}

			// Checks particles and updates neighborlist if necessary.
			inline virtual void CheckNeighborListUpdate(const ParticleList& particles) override
			{
				for(auto& particle : particles)
				{
					auto dist = particle->GetCheckpointDist();
					ApplyMinimumImage(dist);
					if(dist.norm2() > _rskinsq/4.0)	
						UpdateNeighborList();
				}
			}

			// Applies periodic boundaries to positions.
			inline virtual void ApplyPeriodicBoundaries(Position& position) override
			{
				position.x -= _xlength*ffloor(position.x/_xlength);
				position.y -= _ylength*ffloor(position.y/_ylength);
				position.z -= _zlength*ffloor(position.z/_zlength);
			}

			// Applies minimum image convention to distances.
			inline virtual void ApplyMinimumImage(Position& position) override
			{
				if(position.x > _xlength/2.0)
					position.x -= _xlength;
				else if(position.x < -_xlength/2.0)
					position.x += _xlength;
				
				if(position.y > _ylength/2.0)
					position.y -= _ylength;
				else if(position.y < -_ylength/2.0)
					position.y += _ylength;

				if(position.z > _zlength/2.0)
					position.z -= _zlength;
				else if(position.z < -_zlength/2.0)
					position.z += _zlength;
				/*position.x -= _xlength*anint(position.x/_xlength);
				position.y -= _ylength*anint(position.y/_ylength);
				position.z -= _zlength*anint(position.z/_zlength);*/
			}

			// Sets the neighbor list cutoff radius.
			virtual void SetNeighborRadius(double ncut) override
			{
				_ncutsq = ncut*ncut;
				_ncut = ncut;
			}

			// Gets the neighbor list cutoff radius.
			virtual double GetNeighborRadius() override
			{
				return _ncut; 
			}

			// Set skin thickness for neighbor list re-generation.
			virtual void SetSkinThickness(double x) override
			{
				_rskin = x;
				_rskinsq = x*x;
			}

			// Get skin thickness for neighbor list re-generation.
			virtual double GetSkinThickness() override
			{
				return _rskin;
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
