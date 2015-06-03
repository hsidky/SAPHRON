#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	// Abstract base class for a Monte Carlo move.
	class Move
	{
		public:
			virtual ~Move(){}

			// Get acceptance ratio. 
			virtual double GetAcceptanceRatio() = 0;

			// Reset acceptance ratio.
			virtual void ResetAcceptanceRatio() = 0;

			// Perform the required draw for the move. This involved drawing particles if necessary.
			// If moves are performed on particles, pointers are pushed to the particles 
			// container reference for energy evaluation. If it's a world move that requires no particles,
			// simply resize the container to zero. Move should store result of the draw internally. 
			// The ParticleList is for energy evaluation.
			virtual void Draw(World& world, ParticleList& particles) = 0;

			// Perform a move on draw. If the entire world requires an energy
			// re-evaluation (such as volume scaling) then function returns true. Otherwise, it 
			// returns false.
			virtual bool Perform(World& world, ParticleList& particles) = 0;

			// Undo a move.
			virtual void Undo() = 0;

			// Get move name. 
			virtual std::string GetName() = 0;

			// Clone a move.
			virtual Move* Clone() const = 0;
	};
}
