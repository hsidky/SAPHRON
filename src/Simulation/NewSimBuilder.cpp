#include "NewSimBuilder.h"

using namespace std;

namespace SAPHRON
{
	int DumpErrorsToConsole(const vector<string>& msgs, int notw)
	{
		cout << setw(notw) << right << "\033[1;31mError(s)! See below.\033[0m\n";
		for(auto& msg : msgs)
				cout << " * " << msg << "\n";
		return -1;
	}

	void DumpNoticesToConsole(const vector<string>& msgs, string prefix, int notw)
	{
		cout << setw(notw) << right << "\033[32mOK!\033[0m\n";
		if(msgs.size() == 0)
			return;
		
		for(auto& msg : msgs)
			cout << prefix << " * " << msg << "\n";
	}

	bool NewSimBuilder::BuildSimulation(istream& is)
	{
		Json::Reader reader;
		Json::Value root;
		vector<string> notices;

		// Parse JSON.
		cerr << setw(_msgw) << left << " > Validating JSON...";
		if(!reader.parse(is, root))
		{
			DumpErrorsToConsole({reader.getFormattedErrorMessages()}, _notw);
			return false;
		}
		cout << setw(_notw) << right << "\033[32mOK!\033[0m\n";

		// Build world(s).
		cerr << setw(_msgw) << left << " > Building world(s)...";
		for(auto& jworld : root["worlds"])
		{
			try{
				auto* world = World::Build(jworld);
				_worlds.push_back(world);

				// Print notices.
				auto dim = world->GetBoxVectors();
				notices.push_back("Setting size to [" +  
					to_string(dim.x) + ", " + 
					to_string(dim.y) + ", " + 
					to_string(dim.z) + "] \u212B.");
				
				notices.push_back("Setting neighbor list radius to " + 
					to_string(world->GetNeighborRadius()) + 
					" \u212B.");

				notices.push_back("Setting seed to " + 
					to_string(world->GetSeed()) + ".");

			} catch(BuildException& e) {
				DumpErrorsToConsole(e.GetErrors(), _notw);
				return false;
			} catch(exception& e) {
				DumpErrorsToConsole({e.what()}, _notw);
				return false;
			}
		}

		if(_worlds.size() == 0)
		{
			DumpErrorsToConsole({"No worlds have been specified."}, _notw);
			return false;
		}
		DumpNoticesToConsole(notices, "",_notw);
		notices.clear();


		// Build particle(s).
		map<string, int> pcount;
		cerr << setw(_msgw) << left << " > Building particle(s)...";
		
		try {
			Particle::BuildParticles(root["particles"], root["components"], _particles);
		} catch(BuildException& e) {
			DumpErrorsToConsole(e.GetErrors(), _notw);
			return false;
		} catch(exception& e) {
			DumpErrorsToConsole({e.what()}, _notw);
			return false;
		}
		
		if(_particles.size() == 0)
		{
			DumpErrorsToConsole({"No particles have been specified."}, _notw);
			return false;
		}

		// Write particle species notices.
		for(auto& s : pcount)
			notices.push_back("Initialized " + to_string(s.second) + 
							  " particle(s) of type \"" + s.first + "\".");

		DumpNoticesToConsole(notices, "",_notw);
		notices.clear();

		return true;
	}
}