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
		std::vector<int> _cspecies; // Species counter.

	public:
		JSONObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1 );

		virtual void Visit(Ensemble* e) override;
		virtual void Visit(DOSSimulation* e) override;
		virtual void Visit(World* w) override;
		virtual void Visit(Particle* p) override;
		virtual void Visit(MoveManager* mm) override;
		virtual void Visit(ForceFieldManager* fmm) override;

		virtual void PreVisit() override;
		virtual void PostVisit() override;

		~JSONObserver()
		{
			if(_jsonfs)
				_jsonfs->close();
		}
	};
}