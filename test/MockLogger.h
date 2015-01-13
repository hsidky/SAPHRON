#pragma once

#include "../src/Loggers/Logger.h"
#include "../src/Models/BaseModel.h"

using namespace Loggers;

class MockLogger : public Logger
{
	public:
		void LogThermalProperties(BaseModel& model)
		{
			for(auto &prop : this->ThermalProps)
				prop(model);
		}
};
