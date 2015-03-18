#pragma once

#include "Visitor.h"

namespace SAPHRON
{
	class Visitable
	{
		public:
			virtual void AcceptVisitor(class Visitor &v) = 0;
	};
}
