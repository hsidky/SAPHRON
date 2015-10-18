#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Simulation;
	class DOSSimulation;
	class WorldManager;
	class Particle;
	class MoveManager;
	class ForceFieldManager;
	class Histogram;

	class Visitor
	{
		// Abstract base class for a visiting object that traverses visitables.
		public:
			virtual void Visit(const Simulation& e) = 0;
			virtual void Visit(const DOSSimulation& e) = 0;
			virtual void Visit(const WorldManager& wm) = 0;
			virtual void Visit(const Particle& p) = 0;
			virtual void Visit(const MoveManager& mm) = 0;
			virtual void Visit(const ForceFieldManager& ffm) = 0;
			virtual void Visit(const Histogram& hist) = 0;
	};
}
