#pragma once

#include "Visitor.h"

namespace Visitors
{
	class Visitable
	{
		public:
			virtual void AcceptVisitor(class Visitor &v) = 0;
	};
}
