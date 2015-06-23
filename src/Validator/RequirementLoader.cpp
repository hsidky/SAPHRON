#include "RequirementLoader.h"
#include "StringRequirement.h"
#include "IntegerRequirement.h"
#include "NumberRequirement.h"
#include "ObjectRequirement.h"
#include "ArrayRequirement.h"
#include "BooleanRequirement.h"
#include "NullRequirement.h"
#include "EnumRequirement.h"

namespace Json
{
	Requirement* RequirementLoader::LoadRequirement(const Value &json)
	{
		Requirement* item = nullptr;
		// Load up apitemriate requirement type.
		if(json["type"].asString() == "string")
			item = new StringRequirement();
		else if(json["type"].asString() == "integer")
			item = new IntegerRequirement();
		else if(json["type"].asString() == "number")
			item = new NumberRequirement();
		else if(json["type"].asString() == "object")
			item = new ObjectRequirement();
		else if(json["type"].asString() == "array")
			item = new ArrayRequirement();
		else if(json["type"].asString() == "boolean")
			item = new BooleanRequirement();
		else if(json["type"].asString() == "null")
			item = new NullRequirement();
		else if(json["enum"].isArray())
			item = new EnumRequirement();

		return item;
	}

	Requirement* RequirementLoader::LoadRequirement(const ValueType& type)
	{
		Requirement* item = nullptr; 

		switch(type)
		{
			case nullValue:
				item = new NullRequirement();
				break;
			case intValue:
				item = new IntegerRequirement();
				break;
			case realValue:
				item = new NumberRequirement();
				break;
			case stringValue:
				item = new StringRequirement();
				break;
			case booleanValue:
				item = new BooleanRequirement();
				break;
			case arrayValue:
				item = new ArrayRequirement();
				break;
			case objectValue:
				item = new ObjectRequirement();
				break;
			default:
				item = nullptr;
		}

		return item;
	}
}