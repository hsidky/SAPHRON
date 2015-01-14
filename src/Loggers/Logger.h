#pragma once

#include "../Models/BaseModel.h"
#include "../Site.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

using namespace Models;

namespace Loggers
{
	// Abstract base class for property loggers.
	class Logger
	{
		private:
			// Number of calls to the logger.
			int _avgcalls, _thermcalls = 0;

			// Frequency with which to write logs (every "n" iterations).
			int _frequency = 1;

		protected:

			// Functions that evaluate aggreate properties. These are called every
			// iteration. After each sweep, FlushRunningAverages is called to record
			// the data to logger output.
			std::map<std::string, std::function<void(Site&, double&)> > AggregateProps;

			// Functions that evaluate thermal averaged properties. These are aggregates
			// that are calculated after each sweep.
			std::map<std::string, std::function<double(BaseModel&)> > ThermalProps;

			// Vector of running averages averages.
			std::vector<double> RunningAverages;

			// Actual implementation of flushing averages in derived classes.
			virtual void FlushRunningAveragesInternal(int count) = 0;

			// Actual implmenentation of logging thermal properties in derived classes.
			virtual void LogThermalPropertiesInternal(BaseModel& model) = 0;

		public:

			// Instantiate a logger class with a frequecny of 'frequency'- that is,
			// every 'frequency' calls to the logger it will actually log.
			Logger(int frequency = 1) : _frequency(frequency){}

			// Adds a function to the thermal property logger queue.
			void AddThermalProperty(std::string key,
			                        std::function<double(BaseModel&)> prop)
			{
				ThermalProps.insert(
				        std::pair <std::string,
				                   std::function<double(BaseModel&)> >
				                (key, prop));
			}

			// Add an aggregate property to the queue.
			void AddAggregateProperty(std::string key,
			                          std::function<void(Site&, double&)> prop)
			{
				AggregateProps.insert(
				        std::pair <std::string,
				                   std::function<void(Site&, double&)> >
				                (key, prop));

				// Resize thermal averages vector.
				RunningAverages.resize(AggregateProps.size());
			}

			// Run through the thermal properties queue and log functions.
			void LogThermalProperties(BaseModel& model)
			{
				if(_thermcalls++ % _frequency == 0)
					this->LogThermalPropertiesInternal(model);
			};

			// Logs a property of a site each iteration (hence "running"). This is
			// excecuted every iteration after sampling has taken place. "avg" is the
			// variable containing the running property average. After aggregating a
			// running average, it must be flushed to be written to the logger.
			// If a new average is desired, ResetRunningAverages must be called.
			void LogRunningAverages(Site& site)
			{
				int i = 0;
				for(auto &prop : AggregateProps)
				{
					prop.second(site, RunningAverages[i]);
					i++;
				}
			};

			// Resets the running averages.
			void ResetRunningAverages()
			{
				for(size_t i = 0; i < RunningAverages.size(); i++)
					RunningAverages[i] = 0.0;
			}

			// Flush the running average to the log.
			void FlushRunningAverages(int count)
			{
				if(_avgcalls++ % _frequency == 0)
					this->FlushRunningAveragesInternal(count);
			};

			// Resets the call count.
			void ResetCallCount()
			{
				_avgcalls = _thermcalls = 0;
			}
	};
}
