#pragma once

#include "../Models/BaseModel.h"
#include "../Site.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

using namespace Models;

typedef std::map<std::string, const double*> EnsembleProperty;
typedef std::map<std::string, const std::vector<double>*> EnsembleVector;

namespace Loggers
{
	// Abstract base class for property loggers.
	class Logger
	{
		private:
			// Number of calls to the logger.
			int _calls = 0;

			// Frequency with which to write logs (every "n" iterations).
			int _frequency = 1;

		protected:

			// Vector of site property callbacks.
			std::map<std::string,
			         std::function<double(Site&,
			                              const EnsembleProperty&)> >
			SiteProps;

			// Vector of model property callbacks.
			std::map<std::string,
			         std::function<double(BaseModel&,
			                              const EnsembleProperty&)> >
			ModelProps;

			// Vector of vector property callbacks.
			std::map<std::string,
			         std::function<std::vector<double>(BaseModel&,
			                                           const EnsembleVector&)> >
			VectorProps;

			// Vector of ensemble vector properties.
			EnsembleVector EnsembleVecs;

			// Vector of ensemble properties.
			EnsembleProperty EnsembleProps;

			// Actual implmenentation of logging model properties in derived classes.
			virtual void LogModelPropertiesInternal(BaseModel& model) = 0;

			// Actual implementation of logging site properties in derived classes.
			virtual void LogSitePropertiesInternal(Site& site) = 0;

			// Actual implementation of logging vector properties in derived classes.
			virtual void LogVectorPropertiesInternal(BaseModel& model) = 0;

		public:

			// Instantiate a logger class with a frequecny of 'frequency'- that is,
			// every 'frequency' calls to the logger it will actually log.
			Logger(int frequency = 1) : _frequency(frequency){}

			// Adds a function the model properties queue.
			void AddModelProperty(std::string key,
			                      std::function<double(BaseModel&,
			                                           const EnsembleProperty&
			                                           EnsembleProps)>
			                      prop)
			{
				ModelProps.insert(
				        std::pair <std::string,
				                   std::function<double(BaseModel&,
				                                        const EnsembleProperty&
				                                        EnsembleProps)> >
				                (key, prop));
			}

			// Add a site property to the model properties queue.
			void AddSiteProperty(std::string key,
			                     std::function<double(Site&, const EnsembleProperty&
			                                          EnsembleProps)> prop)
			{
				SiteProps.insert(
				        std::pair <std::string,
				                   std::function<double(Site&,
				                                        const EnsembleProperty&
				                                        EnsembleProps)> >
				                (key, prop));
			}

			// Adds a vector property to the vector properties queue.
			void AddVectorProperty(std::string key,
			                       std::function<std::vector<double>(BaseModel&,
			                                                         const
			                                                         EnsembleVector&)>
			                       prop)
			{
				VectorProps.insert(
				        std::pair<std::string,
				                  std::function<std::vector<double>(BaseModel&,
				                                                    const
				                                                    EnsembleVector&)> >(
				                key, prop)
				        );
			}

			// Adds and updates ensemble properties by key.
			void RegisterEnsembleProperty(std::string key, double& value)
			{
				EnsembleProps.insert(std::pair<std::string, double*>(key, &value));
			}

			// Adds and updates ensemble vector properties by key.
			void RegisterEnsembleVectorProperty(std::string key, std::vector<double>& value)
			{
				EnsembleVecs.insert(std::pair<std::string,
				                              std::vector<double>*>(key, &value));
			}

			// Run through the thermal properties queue and log functions.
			// If force is set, logger will run regardless of frequency setting.
			void LogProperties(BaseModel& model, bool force = false)
			{
				if(_calls % _frequency == 0 || force)
				{
					int c = model.GetSiteCount();
					this->LogModelPropertiesInternal(model);
					this->LogVectorPropertiesInternal(model);
					for(int i = 0; i < c; i++)
						this->LogSitePropertiesInternal(
						        *model.SelectSite(i));

					// Fush log
					this->FlushLog();
				}
				_calls++;
			};

			// Flush the running average to the log.
			virtual void FlushLog() = 0;

			// Gets the number of times logger was called.
			int GetCallCount()
			{
				return _calls;
			}

			// Resets the call count.
			void ResetCallCount()
			{
				_calls = 0;
			}
	};
}
