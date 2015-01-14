#pragma once

#include "../src/Loggers/Logger.h"
#include "../src/Models/BaseModel.h"
#include <iostream>

using namespace Loggers;

class MockLogger : public Logger
{
	protected:
		void LogModelPropertiesInternal(BaseModel& model)
		{
			for(auto &prop : this->ModelProps)
				prop.second(model);
		}

		void LogSitePropertiesInternal(Site& site, bool end)
		{
			// Print thermal averages.
			for(auto &prop : this->SiteProps)
				prop.second(site);
		}

	public:
		void FlushLog()
		{
		}
};
