#pragma once 

#include "../Simulation/SimObserver.h"
#include "json/json.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <map>

namespace SAPHRON
{
	// XYZ observer class that creates XYZ snapshots of the simulations.
	class XYZObserver : public SimObserver
	{
	private:
		std::map<int, std::unique_ptr<std::ofstream>> _worldfs;
		std::string _prefix;
		bool _initialized;

		void InitializeXYZ(const WorldManager& wm);

	public:
		XYZObserver(const std::string& prefix, 
					const SimFlags& flags, 
					unsigned int frequency = 1);

		virtual std::string GetName() const override { return "XYZ"; }

		virtual void Visit(const Simulation& e) override;
		virtual void Visit(const DOSSimulation& e) override;
		virtual void Visit(const WorldManager& wm) override;
		virtual void Visit(const MoveManager& mm) override;
		virtual void Visit(const ForceFieldManager& ffm) override;
		virtual void Visit(const Particle& p) override;
		virtual void Visit(const Histogram& hist) override;

		virtual void Serialize(Json::Value& json) const override;

		virtual void PostVisit() override
		{
			_initialized = true;
		}

		~XYZObserver()
		{
			for(auto& w : _worldfs)
				w.second->close();
		}

	};
}