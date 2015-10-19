#pragma once

#include "../Particles/Particle.h"
#include "../Utils/Rand.h"
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
			
		// Box vectors.
		double _xlength, _ylength, _zlength;



		public:
			SimpleWorld(double xlength, double ylength, double zlength, double rcut, int seed = 1) : 
			World(rcut, seed), _xlength(xlength), _ylength(ylength), _zlength(zlength)
			{}

			// Packs a SimpleWorld with the given particle blueprints and 
			// compositions to with "count" total particles and a specified density.
			void PackWorld(const std::vector<Particle*> particles,
			                      		  const std::vector<double> fractions, 
			                      		  int count, double density);

			// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
			// initialized with the appropriate composition.  If max is set, it will stop at that number.
			void ConfigureParticles(const std::vector<Particle*> particles,
			                        const std::vector<double> fractions, int max = 0);

			int GetLatticeSize()
			{
				return _xlength*_ylength*_zlength;
			}

			virtual double GetVolume() const override
			{
				return _xlength*_ylength*_zlength;
			}	

			// Get box vectors.
			virtual Position GetBoxVectors() const override
			{
				return Position{_xlength, _ylength, _zlength};
			}

			// Applies periodic boundaries to positions.
			virtual void ApplyPeriodicBoundaries(Position* position) const override
			{
				(*position)[0] -= _xlength*ffloor((*position)[0]/_xlength);
				(*position)[1] -= _ylength*ffloor((*position)[1]/_ylength);
				(*position)[2] -= _zlength*ffloor((*position)[2]/_zlength);
			}

			virtual void ApplyMinimumImage(Position& position) const override
			{
				if(position[0] > _xlength/2.0)
					position[0] -= _xlength;
				else if(position[0] < -_xlength/2.0)
					position[0] += _xlength;
				
				if(position[1] > _ylength/2.0)
					position[1] -= _ylength;
				else if(position[1] < -_ylength/2.0)
					position[1] += _ylength;

				if(position[2] > _zlength/2.0)
					position[2] -= _zlength;
				else if(position[2] < -_zlength/2.0)
					position[2] += _zlength;
				/*position[0] -= _xlength*anint(position[0]/_xlength);
				position[1] -= _ylength*anint(position[1]/_ylength);
				position[2] -= _zlength*anint(position[2]/_zlength);*/
			}

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

					#pragma omp parallel for
					for(auto it = World::begin(); it < World::end(); ++it)
					{
						const auto& pos = (*it)->GetPositionRef();
						(*it)->SetPosition(xs*pos[0], ys*pos[1], zs*pos[2]);
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

					#pragma omp parallel for
					for(auto it = World::begin(); it < World::end(); ++it)
					{
						auto pos = (*it)->GetPosition();
						ApplyPeriodicBoundaries(&pos);
						(*it)->SetPosition(pos);
					}
				}

				// Regenerate neighbor list.
				UpdateNeighborList();
			}
	

			~SimpleWorld() {}
	};
}
