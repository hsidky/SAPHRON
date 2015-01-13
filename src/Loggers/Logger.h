#pragma once

#include "../Models/BaseModel.h"
#include <functional>
#include <vector>

using namespace Models;

namespace Loggers
{
	// Abstract base class for property loggers.
	class Logger
	{
		protected:

			// Functions that evaluate thermal averaged properties.
			std::vector<std::function<double(BaseModel&)> > ThermalProps;

		public:

			// Adds a function to the property logger queue.
			void AddLoggableProperty(std::function<double(BaseModel&)> prop)
			{
				ThermalProps.push_back(prop);
			}

			// Run through the thermal properties queue and log functions.
			virtual void LogThermalProperties(BaseModel& model) = 0;
	};
}
