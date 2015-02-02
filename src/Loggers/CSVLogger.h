#pragma once

#include "Logger.h"
#include <fstream>
#include <iomanip>
#include <iostream>

namespace Loggers
{
	class CSVLogger : public Logger
	{
		private:
			std::ofstream _modelfs;
			std::ofstream _sitefs;
			std::ofstream _vecfs;

		protected:
			void LogModelPropertiesInternal(BaseModel& model)
			{
				int m = this->ModelProps.size();
				if(m < 1)
					return;

				_modelfs << this->GetCallCount() << ",";
				int i = this->ModelProps.size();
				for(auto &prop : this->ModelProps)
					_modelfs << prop.second(model, this->EnsembleProps)
					         << ((--i > 0) ? "," : "\n");
			}

			void LogSitePropertiesInternal(Site& site)
			{
				int m = this->SiteProps.size();
				if(m < 1)
					return;

				_sitefs << this->GetCallCount() << ",";

				// Write site properties.
				int i = 0;
				for(auto &prop : this->SiteProps)
				{
					_sitefs << prop.second(site, this->EnsembleProps)
					        << ((i < m - 1) ? "," : "\n");
					i++;
				}
			}

			void LogVectorPropertiesInternal(BaseModel& model)
			{
				int m = this->VectorProps.size();
				if(m < 1)
					return;

				_vecfs << this->GetCallCount() << ",";
				int i = this->VectorProps.size();
				for(auto &prop : this->VectorProps)
				{
					auto vals = prop.second(model, this->EnsembleVecs);
					int j = vals.size();
					for(auto &val : vals)
						_vecfs << val << ((--i > 0 || --j > 0) ? "," : "\n");
				}
			}

		public:
			CSVLogger(std::string modelfile,
			          std::string sitefile,
			          std::string vecfile,
			          int frequency = 1)
				: Logger(frequency), _modelfs(modelfile),
				  _sitefs(sitefile), _vecfs(vecfile)
			{
				_modelfs << std::setprecision(20);
				_sitefs << std::setprecision(20);
				_vecfs << std::setprecision(20);
			}

			// Write headers to the beginning of the file.
			void WriteHeaders()
			{
				// Iteration
				_modelfs << "Iteration,";
				_sitefs <<  "Iteration,";
				_vecfs << "Iteration,";

				// Vector properties
				int i = this->VectorProps.size();
				for(auto &prop : this->VectorProps)
					_vecfs << prop.first << ((--i > 0) ? "," : "\n");

				// Model properties
				i = this->ModelProps.size();
				for(auto &prop : this->ModelProps)
					_modelfs << prop.first << ((--i > 0) ? "," : "\n");

				// Site properties.
				i = this->SiteProps.size();
				for(auto &prop : this->SiteProps)
					_sitefs << prop.first << ((--i > 0) ? "," : "\n");
				;
			}

			void FlushLog()
			{
			}

			~CSVLogger()
			{
				_modelfs.close();
				_sitefs.close();
				_vecfs.close();
			}
	};
}
