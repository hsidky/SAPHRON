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
					std::cout << "Magnetization: " << prop(model) << std::endl;
			}
	};
}
