#pragma once

#include "../Models/BaseModel.h"
#include "Logger.h"
#include <iostream>

namespace Loggers
{
	class ConsoleLogger : public Logger
	{
		public:
			void LogThermalPropertiesInternal(BaseModel& model)
			{
				int i = this->ThermalProps.size();
				for(auto &prop : this->ThermalProps)
					std::cout << prop.first << ": "
					          << prop.second(model)
					          << ((--i > 0) ? ", " : "\n");
			}

			void FlushRunningAveragesInternal(int count)
			{
				for(double& avg : this->RunningAverages)
					avg /= count;

				// Print thermal averages.
				int i = 0, m = this->AggregateProps.size();
				for(auto &prop : this->AggregateProps)
				{
					std::cout << prop.first << ": "
					          << this->RunningAverages[i]
					          << ((i < m - 1) ? ", " : "\n");
					i++;
				}

				this->ResetRunningAverages();
			}
	};
}
