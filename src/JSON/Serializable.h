#pragma once 

#include "json/json.h"

namespace SAPHRON
{
	class Serializable
	{

	public:

	// Serialize the class. Write to root relative
	// to base path.
	virtual void Serialize(Json::Value& root) const = 0;
	};
}