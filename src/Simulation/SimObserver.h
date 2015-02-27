#pragma once

#include "../Visitors/Visitor.h"
#include "SimEvent.h"

using namespace Visitors;

namespace Simulation
{
	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public Visitor
	{
		private:
			unsigned int _frequency = 1;
			int _iteration = 0;

		protected:
			// Get iteration count.
			int GetIteration()
			{
				return _iteration;
			}

			// Set iteration count.
			int SetIteration(int i)
			{
				return _iteration = i;
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

			bool IsObservableIteration()
			{
				return _iteration % _frequency == 0;
			}

		public:

			// Initialize a SimObserver class with a specified observation frequency.
			SimObserver(unsigned int frequency = 1) : _frequency(frequency){}

			// Update observer when simulation has changed.
			virtual void Update(SimEvent& ev) = 0;
	};
}
