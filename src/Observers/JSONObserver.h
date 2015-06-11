#pragma once 

#include "../Simulation/SimObserver.h"
#include "json/json.h"
#include <iostream>
#include <fstream>
#include <memory>

namespace SAPHRON
{
	// JSON Observer class that creates a snapshot of the simulation and dumps it to a JSON file.
	class JSONObserver : public SimObserver
	{
		private:
			std::unique_ptr<std::ofstream> _jsonfs;
			unsigned int _counter;
			std::string _prefix;
			Json::Value _root;

		public:
			JSONObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1 );

			virtual void Visit(Ensemble* e) override;
			virtual void Visit(DOSEnsemble* e) override;
			virtual void Visit(World* w) override;
			virtual void Visit(Particle* p) override;
			virtual void Visit(MoveManager* mm) override;

			virtual void PreVisit() override;
			virtual void PostVisit() override;

			~JSONObserver()
			{
				if(_jsonfs)
					_jsonfs->close();
			}
	};
}