#pragma once 

#include "json/json.h"
#include "Requirement.h"

namespace Json
{
	class RequirementLoader
	{
	public:
		Requirement* LoadRequirement(Value json);
	};
}