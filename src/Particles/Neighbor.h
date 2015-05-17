#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Particle;

	// Class representing a neighbor to a particle.
	class Neighbor
	{
		private:
			Particle* _particle;

		public:
			Neighbor(Particle& particle) : _particle(&particle){}

			// Gets the neighboring particle.
			Particle* GetParticle()
			{
				return _particle;
			}
	};
}
