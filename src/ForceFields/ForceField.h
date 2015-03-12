#pragma once
#include "../Particles/Particle.h"

namespace SAPHRON
{
	// Abstract base class for a force field. Represents the scalar interaction potential between
	// two bodies (particles).
	class ForceField
	{
		// Evaluates the potential between two particles.
		virtual double Evaluate(const Particle& p1, const Particle& p2) = 0;
	};
}
