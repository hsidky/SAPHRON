#include "XYZObserver.h"
#include "../Particles/Particle.h"
#include "../Simulation/Simulation.h"
#include "../Simulation/DOSSimulation.h"
#include "../Worlds/WorldManager.h"
#include "../Moves/MoveManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Worlds/World.h"
#include "../Utils/Histogram.h"
#include <iomanip>

using namespace std;

namespace SAPHRON
{
	XYZObserver::XYZObserver(const std::string& prefix,
							 const SimFlags& flags,
							 unsigned int frequency) : 
	SimObserver(flags, frequency), _prefix(prefix), _initialized(false)
	{
	}

	void XYZObserver::InitializeXYZ(const WorldManager& wm)
	{
		for(const auto& w : wm)
		{
			_worldfs[w->GetID()] = unique_ptr<ofstream>(
				new ofstream(_prefix + "." + w->GetStringID() + ".xyz")
			);

			auto& fs = _worldfs[w->GetID()];
			fs->precision(8);
			*fs << fixed << left;
		}
	}

	void XYZObserver::Visit(const Simulation&) {}

	void XYZObserver::Visit(const DOSSimulation&) {}

	void XYZObserver::Visit(const WorldManager& wm)
	{
		if(!_initialized)
			InitializeXYZ(wm);

		for(auto& w : wm)
		{
			auto& fs = _worldfs[w->GetID()];
			*fs << w->GetPrimitiveCount() << "\n";
			auto& H = w->GetHMatrix();
			*fs << H(0,0) << " " << H(1,1) << " " << H(2,2) << "\n";

			for(int i = 0; i < w->GetPrimitiveCount(); ++i)
			{
				auto* p = w->SelectPrimitive(i);
				auto& pos = p->GetPosition();
				auto& dir = p->GetDirector();
				*fs << setw(10) << p->GetSpecies() << " " 
					<< setw(16) << pos[0] << " " 
					<< setw(16) << pos[1] << " "
					<< setw(16) << pos[2] << " "
					<< setw(16) << dir[0] << " "
					<< setw(16) << dir[1] << " "
					<< setw(16) << dir[2] << "\n";
			}
		}
	}

	void XYZObserver::Visit(const ForceFieldManager&) {}

	void XYZObserver::Visit(const MoveManager&) {}

	void XYZObserver::Visit(const SAPHRON::Particle&) {}

	void XYZObserver::Visit(const SAPHRON::Histogram&)	{}

	void XYZObserver::Serialize(Json::Value& json) const
	{
		json["type"] = "XYZ";
		json["frequency"] = GetFrequency();
		json["prefix"] = _prefix;
	}

}