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
				for(auto &prop : this->ThermalProps)
					prop(model);
			}

			void FlushRunningAverages(int count)
			{
				for(double& avg : this->ThermalAverages)
				{
						avg /= count;
				}
			}
	};
}
