#pragma once

namespace SAPHRON
{
	// Forward declare.
	class Ensemble;

	class Visitor
	{
		// Abstract base class for a visiting object that traverses visitables.
		public:
			virtual void Visit(Ensemble* e) = 0;
	};
}
