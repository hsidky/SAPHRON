#pragma once 

#include "json/json.h"
#include "Requirement.h"

namespace Json
{
	class RequirementLoader
	{
	public:
		Requirement* LoadRequirement(const Value& json);

		Requirement* LoadExtended(const Value& json);
		
		Requirement* LoadRequirement(const ValueType& type);
	};
}