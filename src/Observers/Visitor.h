#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Ensemble;
	class DOSEnsemble;
	class WorldManager;
	class Particle;
	class MoveManager;
	class ForceFieldManager;

	class Visitor
	{
		// Abstract base class for a visiting object that traverses visitables.
		public:
			virtual void Visit(const Ensemble& e) = 0;
			virtual void Visit(const DOSEnsemble& e) = 0;
			virtual void Visit(const WorldManager& wm) = 0;
			virtual void Visit(const Particle& p) = 0;
			virtual void Visit(const MoveManager& mm) = 0;
			virtual void Visit(const ForceFieldManager& ffm) = 0;
	};
}
