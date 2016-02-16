#include "JSONObserver.h"
#include "../Particles/Particle.h"
#include "../Simulation/Simulation.h"
#include "../Simulation/DOSSimulation.h"
#include "../Worlds/WorldManager.h"
#include "../Moves/MoveManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Worlds/World.h"
#include "../Utils/Histogram.h"
#include <ctime>
#include <iomanip>

namespace SAPHRON
{
	std::string GetTimestamp()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time (&rawtime);
		timeinfo = localtime(&rawtime);

  		strftime(buffer,80,"%Y-%m-%d %r",timeinfo);
  		return std::string(buffer);
	}

	JSONObserver::JSONObserver(const std::string& prefix, 
							   const SimFlags& flags, 
							   unsigned int frequency) : 
	SimObserver(flags, frequency), _prefix(prefix), _root()
	{
	}

	void JSONObserver::PreVisit()
	{
		_jsonfs = std::unique_ptr<std::ofstream>(
			new std::ofstream(_prefix + ".json")
		);

		_jsonfs->precision(20);
	}

	void JSONObserver::PostVisit()
	{
		*_jsonfs 
		<< "/**********************************************\n"
		<< " *           SAPHRON JSON Snapshot            *\n"
		<< " *      Generated " << GetTimestamp() << "      *\n"
		<< " **********************************************/\n";

		Json::StreamWriterBuilder builder;
		builder["commentStyle"] = "None";
		builder["indentation"] = "\t";
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(_root, &*_jsonfs);
		*_jsonfs << std::endl;

		_jsonfs->close();
		_root.clear();
	}

	void JSONObserver::Visit(const Simulation& e)
	{
		e.Serialize(_root);
	}

	void JSONObserver::Visit(const DOSSimulation& e)
	{
		e.Serialize(_root);
	}

	void JSONObserver::Visit(const WorldManager& wm)
	{
		wm.Serialize(_root);
	}

	void JSONObserver::Visit(const ForceFieldManager& ffm)
	{
		ffm.Serialize(_root);
	}

	void JSONObserver::Visit(const MoveManager& mm)
	{
		mm.Serialize(_root);
	}

	void JSONObserver::Visit(const SAPHRON::Particle&) {}

	void JSONObserver::Visit(const SAPHRON::Histogram& hist)
	{
		hist.Serialize(_root["histogram"]);
	}

	void JSONObserver::Serialize(Json::Value& json) const
	{
		json["type"] = "JSON";
		json["frequency"] = GetFrequency();
		json["prefix"] = _prefix;
	}

}