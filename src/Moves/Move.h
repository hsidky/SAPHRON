#pragma once

#include "../Particles/Particle.h"

namespace SAPHRON
{
	// Abstract base class for a Monte Carlo move.
	class Move
	{
		public:
			virtual ~Move(){}

			// Get required number of particles.
			virtual unsigned int RequiredParticles() { return 1; }

			// Get acceptance ratio. 
			virtual double GetAcceptanceRatio() = 0;

			// Reset acceptance ratio.
			virtual void ResetAcceptanceRatio() = 0;

			// Perform a move.
			virtual void Perform(const ParticleList& particles) = 0;

			// Undo a move.
			virtual void Undo() = 0;

			// Get move name. 
			virtual std::string GetName() = 0;

			// Clone a move.
			virtual Move* Clone() const = 0;
	};
}
