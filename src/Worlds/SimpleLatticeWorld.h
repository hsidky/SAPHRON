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
			std::vector<ParticlePtr> _particles;
			Rand _rand;

		public:
			SimpleLatticeWorld(int xlength, int ylength, int zlength, int seed = 1)
				: _xlength(xlength), _ylength(ylength), _zlength(zlength), _rand(seed)
			{
			}

			// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
			// initialized with the appropriate composition.
			void ConfigureParticles(const std::vector<Particle*> particles,
			                        const std::vector<double> fractions)
			{
				if(particles.size() != fractions.size())
				{
					std::cerr << "ERROR: Particle and fraction count mismatch." << std::endl;
					exit(-1);
				}

				auto fracs = fractions;
				int cnt = (int)particles.size();

				// Normalize
				double norm = 0;
				for(int i = 0; i < cnt; i++)
					norm += fracs[i];

				for(int i = 0; i < cnt; i++)
					fracs[i] /= norm;

				// Initialize counts.
				std::vector<int> counts(cnt);
				for(int i = 0; i < cnt-1; i++)
					counts[i] = (int)std::round(fracs[i]*GetLatticeSize());

				counts[cnt-1] = GetLatticeSize();
				for(int i = 0; i < cnt-1; i++)
					counts[cnt-1] -= counts[i];

				// Loop though and initialize
				double x = 1;
				double y = 1;
				double z = 1;
				for(int i = 0; i < GetLatticeSize(); i++)
				{
					if(z > _zlength)
						z = 1.0;
					if(y > _ylength)
						y = 1.0;
					if(x > _xlength)
						x = 1.0;

					for(int j = 0; j < cnt; j++)
						if(counts[j] > 0 )
						{
							_particles.emplace_back(particles[j]->Clone());
							_particles[i]->SetPosition({x,y,z});
							counts[j]--;
							break;
						}

					// Increment counters.
					x += floor((y + z) / (_zlength + _ylength));
					y += floor(z / _zlength);
					z += 1.0;
				}
			}

			// Draw a random particle from the world.
			virtual Particle* DrawRandomParticle() override
			{
				return _particles[_rand.int32() % this->_particles.size()].get();
			}

			// Get number of high level particles in the world.
			virtual int GetParticleCount() override
			{
				return (int)_particles.size();
			}

			// Get a particle by index.
			virtual Particle* SelectParticle(int location) override
			{
				return _particles[location].get();
			}

			int GetLatticeSize()
			{
				return _xlength*_ylength*_zlength;
			}

			void ConfigureNeighborList()
			{
				if((int)_particles.size() != GetLatticeSize())
				{
					std::cerr << "ERROR: Please configure particles first." << std::endl;
					exit(-1);
				}

				for(int i = 0; i < GetLatticeSize()- 1; i++)
				{
					auto ni = _particles[i];
					auto ci = ni->GetPosition();

					for(int j = i + 1; j < GetLatticeSize(); j++)
					{
						auto nj = _particles[j];
						auto cj = nj->GetPosition();

						Position rij;
						rij.x = std::abs(ci.x - cj.x);
						rij.y = std::abs(ci.y - cj.y);
						rij.z = std::abs(ci.z - cj.z);

						if(rij.x == _xlength-1)
							rij.x = 1;
						if(rij.y == _ylength-1)
							rij.y = 1;
						if(rij.z == _xlength-1)
							rij.z = 1;

						if((rij.x + rij.y +rij.z) == 1)
						{
							ni->AddNeighbor(Neighbor(*nj.get()));
							nj->AddNeighbor(Neighbor(*ni.get()));
						}
					}
				}
			}
	};
}
