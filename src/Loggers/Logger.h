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
		protected:

			// Functions that evaluate aggreate properties. These are called every
			// iteration. After each sweep, FlushRunningAverages is called to record
			// the data to logger output.
			std::map<std::string, std::function<void(Site&, double&)> > AggregateProps;

			// Functions that evaluate thermal averaged properties. These are aggregates
			// that are calculated after each sweep.
			std::map<std::string, std::function<double(BaseModel&)> > ThermalProps;

			// Vector of running averages averages.
			std::vector<double> ThermalAverages;

		public:

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
				ThermalAverages.resize(AggregateProps.size());
			}

			// Run through the thermal properties queue and log functions.
			virtual void LogThermalProperties(BaseModel& model) = 0;

			// Logs a property of a site each iteration (hence "running"). This is
			// excecuted every iteration after sampling has taken place. "avg" is the
			// variable containing the running property average.
			virtual void LogRunningAverages(Site& site)
			{
				int i = 0;
				for(auto &prop : AggregateProps)
				{
					prop.second(site, ThermalAverages[i]);
					i++;
				}
			};

			// Resets the running average.
			virtual void ResetRunningAverages()
			{
				for(size_t i = 0; i < ThermalAverages.size(); i++)
					ThermalAverages[i] = 0.0;
			}

			// Flush the running average to the log.
			virtual void FlushRunningAverages(int count) = 0;
	};
}
