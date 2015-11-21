#pragma once 

#include <chrono>
#include <map>
#include <iostream>
// Based on code provided by Daniel Brake.

namespace SAPHRON
{
	// The TimerData class is for use in conjunction with the Timer class below, via a map. 
	// It holds elapsed time, a start time, and the number of times it has been incremented.
	// The methods for adjusting these are all set in the timer class. This is a POD.
	class TimerData
	{
	public:
		// Start time. Set by pushing start.
		std::chrono::high_resolution_clock::time_point t1; 

		// Total elapsed time so far. Incremented by adding time. 
		std::chrono::milliseconds elapsed_time;

		// How many times this category has been timed.
		int num_calls_timed;

		TimerData() :
		elapsed_time(std::chrono::milliseconds(0)), num_calls_timed(0)
		{
		}
	};

	using TimerMap = std::map<std::string, TimerData>;

	// This Timer class keeps track of timing data. It uses a map to hold actual time elapsed
	// and the number of times the timer has been incremeneted. It is capable of timinng 
	// arbitrary categories of processes, using a map of TimerData's.
	class Timer
	{
	private:
		// Map of TimerData objects.
		TimerMap _timers;

		// Add a timer to the map. Returns true if a new 
		// timer is created, false if it already exists.
		bool CreateTimer(const std::string& name)
		{
			if(_timers.find(name) == _timers.end())
			{
				_timers[name] = TimerData();
				return true;
			}
			else
				return false;
		}

	public:
		Timer() : 
		_timers()
		{
			PressStart("total");
		}

		// Starts the timer for name.
		void PressStart(const std::string& name)
		{
			CreateTimer(name);
			_timers[name].t1 = std::chrono::high_resolution_clock::now();
		}

		// Adds time to the a particular timer.
		// Increments the elapsed time and counter by 1. 
		void AddTime(const std::string& name)
		{
			if(CreateTimer(name))
			{
				std::cerr << "Tried to add time to a counter which didn't exist!" << std::endl;
				exit(-1);
			}

			_timers[name].elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - _timers[name].t1
			);

			++_timers[name].num_calls_timed;
		}

		// Get timer data from name.
		const TimerData& GetTimerData(const std::string& name) const
		{
			return _timers.at(name);
		}

		// Get timer map.
		const TimerMap& GetTimerMap() const { return _timers; }
	};
}