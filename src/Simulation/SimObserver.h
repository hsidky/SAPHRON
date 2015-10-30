#pragma once

#include "../Observers/Visitor.h"
#include "../JSON/Serializable.h"
#include "SimEvent.h"
#include <mutex>
#include <vector>
#include "json/json.h"

namespace SAPHRON
{
	class SimObserver; 
	typedef std::vector<SimObserver*> ObserverList;

	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public Visitor, public Serializable
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
			void Update(const SimEvent& e);

			// Get flags.
			SimFlags GetFlags() const { return Flags; }

			// Get logging frequency.
			unsigned int GetFrequency() const { return _frequency; }

			// Get name. 
			virtual std::string GetName() const = 0;

			// Called before visitors invokes.
			virtual void PreVisit(){}

			// Called after visitors complete.
			virtual void PostVisit(){}

			// Serialize observer.
			virtual void Serialize(Json::Value& json) const = 0;

			// Static builder method for simobserver. If return value is nullptr, 
			// then an unknown error occurred. It will throw a BuildException on 
			// failure.  Object lifetime is caller's responsibility!
			static SimObserver* BuildObserver(const Json::Value& json, const std::string& path);
			
			// Static builder method for simobserver. If return value is nullptr, 
			// then an unknown error occurred. It will throw a BuildException on 
			// failure.  Object lifetime is caller's responsibility!
			static SimObserver* BuildObserver(const Json::Value& json);
			
			// Static builder method for sim observers. ObserverList is a vector
			// which will be populated with initialized observers. Object lifetime
			// is caller's responsibility!
			static void BuildObservers(const Json::Value& json, ObserverList& ol);

			virtual ~SimObserver(){}	
	};
}
