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
		std::string _prefix;
		Json::Value _root;

	public:
		JSONObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1 );

				// Get Observer name.
		virtual std::string GetName() const override{ return "JSON"; }

		virtual void Visit(const Simulation& e) override;
		virtual void Visit(const DOSSimulation& e) override;
		virtual void Visit(const WorldManager& wm) override;
		virtual void Visit(const MoveManager& mm) override;
		virtual void Visit(const ForceFieldManager& ffm) override;
		virtual void Visit(const Particle& p) override;
		virtual void Visit(const Histogram& hist) override;

		virtual void Serialize(Json::Value& json) const override;
		
		virtual void PreVisit() override;
		virtual void PostVisit() override;

		~JSONObserver()
		{
			if(_jsonfs)
				_jsonfs->close();
		}
	};
}