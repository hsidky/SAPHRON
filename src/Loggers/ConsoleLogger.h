#pragma once

#include "../Models/BaseModel.h"
#include "Logger.h"
#include <iostream>

namespace Loggers
{
	class ConsoleLogger : public Logger
	{
		public:
			void LogThermalProperties(BaseModel& model)
			{
				int i = this->ThermalProps.size();
				for(auto &prop : this->ThermalProps)
					std::cout << prop.first << ": "
					          << prop.second(model)
					          << ((--i > 0) ? ", " : "\n");
			}

			void FlushRunningAverages(int count)
			{
				for(double& avg : this->ThermalAverages)
					avg /= count;

				// Print thermal averages.
				int i = 0, m = this->AggregateProps.size();
				for(auto &prop : this->AggregateProps)
				{
					std::cout << prop.first << ": "
					          << this->ThermalAverages[i]
					          << ((i < m - 1) ? ", " : "\n");
					i++;
				}
			}
	};
}
