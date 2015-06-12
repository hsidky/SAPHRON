#pragma once 

#include "Move.h"

namespace SAPHRON
{
	// Abstract base class that extends move for Gibbs ensemble moves where 
	// moves may happen on multiple worlds.
	class GibbsMove : public Move
	{
	public:

		// Perform the required draw for the move. This involved drawing particles if necessary.
		// If moves are performed on particles, pointers are pushed to the particles 
		// container reference for energy evaluation. If it's a world move that requires no particles,
		// simply resize the container to zero.
		virtual void Draw(World& world, ParticleList& particles) = 0;

		// Perform a move on draw. If the entire world requires an energy
		// re-evaluation (such as volume scaling) then function returns true. Otherwise, it 
		// returns false.
		virtual bool Perform(World& world, ParticleList& particles) = 0;

		// Similar to Draw above except that it operates on multiple worlds. 
		virtual void Draw(const WorldList& worlds, ParticleList& particles) = 0;

		// Similar to perform above except that it operates on multiple worlds.
		virtual bool Perform(const WorldList& worlds, ParticleList& particles) = 0;
	};
}