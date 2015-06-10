#pragma once

#include "../Observers/Visitor.h"
#include "SimEvent.h"
#include <mutex>

namespace SAPHRON
{
	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public Visitor
	{
		private:
			unsigned int _frequency = 1;
			SimEvent _event;
			std::mutex _mutex;

		protected:

			const SimFlags Flags;

			SimEvent& GetEvent()
			{
				return _event;
			}

			// Get logging frequency.
			unsigned int GetFrequency()
			{
				return _frequency;
			}

			// Set logging frequency.
			unsigned int SetFrequency(int f)
			{
				return _frequency = f;
			}

			// Get current event iteration
			unsigned int GetIteration()
			{
				return _event.GetIteration();
			}

			// Get caller identifier.
			int GetObservableID()
			{
				return 0;
			}

		public:

			// Initialize a SimObserver class with a specified observation frequency.
			SimObserver(SimFlags flags, unsigned int frequency = 1)
				: _frequency(frequency), _event(nullptr, 0), Flags(flags){}

			// Update observer when simulation has changed.
			void Update(SimEvent& e);

			virtual ~SimObserver(){}

	};
}
