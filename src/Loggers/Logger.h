#pragma once

#include "../Models/BaseModel.h"
#include "../Site.h"
#include <functional>
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
			std::vector<std::function<void(Site&, double&)> > AggregateProps;

			// Functions that evaluate thermal averaged properties. These are aggregates
			// that are calculated after each sweep.
			std::vector<std::function<double(BaseModel&)> > ThermalProps;

			// Vector of running averages averages.
			std::vector<double> ThermalAverages;

		public:

			// Adds a function to the thermal property logger queue.
			void AddThermalProperty(std::function<double(BaseModel&)> prop)
			{
				ThermalProps.push_back(prop);

			}

			void AddAggregateProperty(std::function<void(Site&, double&)> prop)
			{
				AggregateProps.push_back(prop);

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
					for(size_t i=0; i < AggregateProps.size(); i++)
							AggregateProps[i](site, ThermalAverages[i]);
			};

			// Resets the running average.
			virtual void ResetRunningAverages()
			{
				for(size_t i = 0; i < ThermalAverages.size(); i++)
					ThermalAverages[i] = 0.0;
			}

			virtual void FlushRunningAverages(int count) = 0;
	};
}
