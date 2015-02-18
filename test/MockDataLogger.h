#pragma once

#include "../src/DataLoggers/DataLogger.h"
#include "../src/Models/BaseModel.h"
#include <iostream>

using namespace DataLoggers;

class MockDataLogger : public DataLogger
{
	protected:
		void LogModelPropertiesInternal(BaseModel& model)
		{
			for(auto &prop : this->ModelProps)
				prop.second(model,  this->EnsembleProps);
		}

		void LogSitePropertiesInternal(Site& site)
		{
			// Print thermal averages.
			for(auto &prop : this->SiteProps)
				prop.second(site, this->EnsembleProps);
		}

		void LogVectorPropertiesInternal(BaseModel& model)
		{
			for(auto &prop : this->VectorProps)
				prop.second(model, this->EnsembleVecs);
		}

		void RegisterLoggableProperties(DataLogger&)
		{
		}

	public:
		void FlushLog()
		{
		}
};
