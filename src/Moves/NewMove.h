#pragma once

#include "../Particles/Particle.h"

namespace SAPHRON
{
	// Abstract base class for a Monte Carlo move.
	class Move
	{
		public:
			virtual ~Move(){}

			// Perform a move.
			virtual void Perform(Particle& particle) = 0;

			// Undo a move.
			virtual void Undo() = 0;

			// Clone a move.
			virtual Move* Clone() const = 0;
	};
}
