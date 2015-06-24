#pragma once 

#include "Requirement.h"

namespace Json
{
	class NullRequirement : public Requirement
	{
	public:
		virtual void Reset() {}

		virtual void Parse(Value, std::string) {}

		virtual void Validate(const Value& json, std::string path)
		{
			if(!json.isNull())
				PushError(path + ": Must be a null value");
		}
	};
}