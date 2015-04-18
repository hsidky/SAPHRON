#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	// Abstract base class for defining new properties. A property is initially calculated from a World
	// object, after which it is updated incrementally by a call to UpdateProperty.
	class Property
	{
		public:
			Property(const World& world){}

			// Update property based on a change in `particle`.
			virtual void UpdateProperty(const ParticleList& particles) = 0;

			virtual ~Property(){}
	};
}
