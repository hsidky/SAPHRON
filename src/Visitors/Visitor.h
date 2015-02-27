#pragma once

#include "../Site.h"

namespace Ensembles
{
	template <class T>
	class NVTEnsemble;
}

namespace Visitors
{
	class Visitor
	{
		public:
			virtual void Visit(Ensembles::NVTEnsemble<Site>* e) = 0;
	};
}
