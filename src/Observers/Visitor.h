#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Ensemble;
	class DOSEnsemble;
	class World;
	class Particle;
	class MoveManager;
	class ForceFieldManager;

	class Visitor
	{
		// Abstract base class for a visiting object that traverses visitables.
		public:
			virtual void Visit(Ensemble* e) = 0;
			virtual void Visit(DOSEnsemble* e) = 0;
			virtual void Visit(World* w) = 0;
			virtual void Visit(Particle* p) = 0;
			virtual void Visit(MoveManager* mm) = 0;
			virtual void Visit(ForceFieldManager* ffm) = 0;
	};
}
