#pragma once

#include "../src/Loggers/Logger.h"
#include "../src/Models/BaseModel.h"
#include <iostream>

using namespace Loggers;

class MockLogger : public Logger
{
	public:
		MockLogger(int frequency = 1) : Logger(frequency){}

		void LogThermalPropertiesInternal(BaseModel& model)
		{
			for(auto &prop : this->ThermalProps)
				prop.second(model);
		}

		void FlushRunningAveragesInternal(int count)
		{
			for(double& avg : this->RunningAverages)
				avg /= count;
		}
};
