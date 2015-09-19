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
			std::vector<Particle*> _particles;
			Rand _rand;

			// Cutoff radius.
			double _rcut, _rcutsq; 

			// Neighbor list cutoff radius.
			double _ncut, _ncutsq;

			// Skin thickness (calculated).
			double _skin, _skinsq;

			// Composition of the system.
			CompositionList _composition;

			// Seed.
			int _seed;

			void AddParticleComposition(Particle* particle);
			void RemoveParticleComposition(Particle* particle);
			void ModifyParticleComposition(const ParticleEvent& pEvent);
			void UpdateNeighborList(Particle* p1, Particle* p2);

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
			virtual void VisitChildren(Visitor& v) const override
			{
				for(auto& particle : _particles)
					particle->AcceptVisitor(v);
			}

		public:
			SimpleWorld(double xlength, double ylength, double zlength, double rcut, int seed = 1) : 
			World(xlength, ylength, zlength), _particles(0), _rand(seed), _rcut(rcut), _rcutsq(rcut*rcut), 
			_ncut(0), _ncutsq(0), _skin(0), _skinsq(0), _composition(), _seed(seed)
			{
				// Compute default neighbor list cutoff and skin thickness. 
				_skin = 0.30 * _rcut;
				_skinsq = _skin * _skin;
				_ncut = _rcut + _skin;
				_ncutsq = (_rcut + _skin)*(_rcut + _skin);
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
				int n = _particles.size();
				if(n == 0)
					return nullptr;

				return _particles[_rand.int32() % n];
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
			}

			// Checks particles and updates neighborlist if necessary.
			inline virtual void CheckNeighborListUpdate(const ParticleList& particles) override
			{
				for(auto& particle : particles)
				{
					auto dist = particle->GetCheckpointDist();
					ApplyMinimumImage(dist);
					if(dot(dist,dist) > _skinsq/4.0)
					{	
						UpdateNeighborList();
						return;
					}
				}
			}

			// Check particle and update neighbor lists if needed.
			virtual void CheckNeighborListUpdate(Particle* p) override
			{
				auto dist = p->GetCheckpointDist();
				ApplyMinimumImage(dist);
				if(dot(dist,dist) > _skinsq/4.0)	
					UpdateNeighborList();
			}

			// Sets the neighbor list cutoff radius.
			virtual void SetNeighborRadius(double ncut) override
			{
				_ncutsq = ncut*ncut;
				_ncut = ncut;

				// Recompute skin.
				_skin = _ncut - _rcut;
				_skinsq = _skin * _skin;
			}

			// Gets the neighbor list cutoff radius.
			virtual double GetNeighborRadius() const override
			{
				return _ncut; 
			}

			// Set cutoff radius for interactions.
			virtual void SetCutoffRadius(double x) override
			{
				_rcut = x;
				_rcutsq = x*x;

				// Recompute skin.
				_skin = _ncut - _rcut;
				_skinsq = _skin * _skin;
			}

			// Get cutoff radius for interactions.
			virtual double GetCutoffRadius() const override
			{
				return _rcut;
			}

			// Get skin thickness for the world.
			virtual double GetSkinThickness() const override
			{
				return _skin;
			}

			// Get seed.
			virtual int GetSeed() const override
			{
				return _seed;
			}

			int GetLatticeSize()
			{
				return _xlength*_ylength*_zlength;
			}

			// Update the neighbor list for all particles in the world.
			virtual void UpdateNeighborList() override;	

			// Update neighbor list for a single particle.
			virtual void UpdateNeighborList(Particle* particle) override;

			// Sets the box vectors (box volume) and if scale is true, scale the coordinates of the 
			// particles in the system. Energy recalculation after this procedure is recommended.
			// The neighbor list is auto regenerated.
			virtual void SetBoxVectors(double x, double y, double z, bool scale) override
			{
				if(scale)
				{
					// First we scale particle positions, then we shrink the box.
					// Calculate scale factor.
					double xs = x/_xlength;
					double ys = y/_ylength;
					double zs = z/_zlength;
					for(auto& particle : _particles)
					{
						const auto& pos = particle->GetPositionRef();
						particle->SetPosition(xs*pos[0], ys*pos[1], zs*pos[2]);
					}

					_xlength = x;
					_ylength = y;
					_zlength = z;
				}
				else
				{
					// Change volume first, then apply PBC.
					_xlength = x;
					_ylength = y;
					_zlength = z;

					for(auto& particle: _particles)
					{
						auto pos = particle->GetPosition();
						ApplyPeriodicBoundaries(&pos);
						particle->SetPosition(pos);
					}
				}

				// Regenerate neighbor list.
				UpdateNeighborList();
			}
	

			~SimpleWorld()
			{
				for(auto& p : _particles)
					delete p;

				_particles.clear();
			}
	};
}
