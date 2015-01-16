#pragma once

#include "../Models/BaseModel.h"
#include "Logger.h"
#include <iostream>

namespace Loggers
{
	class ConsoleLogger : public Logger
	{
		protected:
			void LogModelPropertiesInternal(BaseModel& model)
			{
				int i = this->ModelProps.size();
				for(auto &prop : this->ModelProps)
					std::cout << prop.first << ": "
					          << prop.second(model, this->EnsembleProps)
					          << ((--i > 0) ? ", " : "\n");
			}

			void LogSitePropertiesInternal(Site& site)
			{
				// Print thermal averages.
				int i = 0, m = this->SiteProps.size();
				for(auto &prop : this->SiteProps)
				{
					std::cout << prop.first << ": "
					          << prop.second(site, this->EnsembleProps)
					          << ((i < m - 1) ? ", " : "\n");
					i++;
				}
			}

		public:

			// Use base logger constructor.
			using Logger::Logger;

			void FlushLog()
			{
			}
	};
}
