#pragma once

namespace SAPHRON
{
	// Forward declare.
	class NVTEnsemble;

	class Visitor
	{
		public:
			virtual void Visit(NVTEnsemble* e) = 0;
	};
}
