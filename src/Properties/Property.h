#pragma once 

#include "../Models/BaseModel.h"

namespace Properties
{
	using namespace Models;

	class Property
	{
		public:
			Property(const BaseModel& model){}

			// Calculates property based on a change in model element at index.
			virtual void CalculateProperty(const BaseModel& model, int index) = 0;
			virtual ~Property(){}
	};
}