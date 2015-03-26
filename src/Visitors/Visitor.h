#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Ensemble;
	class World;
	class Particle;

	class Visitor
	{
		// Abstract base class for a visiting object that traverses visitables.
		public:
			virtual void Visit(Ensemble* e) = 0;
			virtual void Visit(World* w) = 0;
			virtual void Visit(Particle* p) = 0;
	};
}
