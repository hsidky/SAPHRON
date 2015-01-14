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
					          << prop.second(model)
					          << ((--i > 0) ? ", " : "\n");
			}

			void LogSitePropertiesInternal(Site& site, bool end)
			{
				// Print thermal averages.
				int i = 0, m = this->SiteProps.size();
				for(auto &prop : this->SiteProps)
				{
					std::cout << prop.first << ": "
					          << prop.second(site)
					          << ((i < m - 1) ? ", " : "\n");
					i++;
				}
			}

		public:
			void FlushLog()
			{
			}
	};
}
