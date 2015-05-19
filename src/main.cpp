// This example is a demonstration of Semi-grand Density of States Sampling
// of a Lebwhol-Lasher binary mixture

// Include header files
// Include for parsing using stringstream
#include <iostream>
#include <sstream>
#include <string>


#include "Worlds/World.h"
#include "Worlds/SimpleWorld.h"
#include "Particles/Site.h"
#include "json/json.h"


inline bool caseInsCharCompareN(char a, char b) {
    return(toupper(a) == toupper(b));
}

bool iequal(const std::string& s1, const std::string& s2) {
	return((s1.size() == s2.size()) &&
    	equal(s1.begin(), s1.end(), s2.begin(), caseInsCharCompareN));
}


using namespace SAPHRON;
// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	Json::Value root;   // 'root' will contain the root value after parsing.
	
	try {
		
		Json::Reader reader;	
		std::cin >> root;

		// Create world. 
		const Json::Value json_world = root["world"];

		if(!json_world)
		{
			std::cerr << "No world specified!" << std::endl;
			return -1;
		}

		auto json_world_size = json_world["size"];
		if(json_world_size.size() != 3)
		{
			std::cerr << "The size needs to be three vectors." << std::endl;
			return -1;
		}

		World* world; 
		if(iequal("SimpleLattice", json_world["type"].asString()))
		{
			world = new SimpleWorld(json_world_size[0].asInt(), 
										   json_world_size[1].asInt(), 
										   json_world_size[2].asInt(),
										   json_world.get("seed", 12345).asInt());
		}
		else 
		{
			std::cerr << "Uknown world specified!" << std::endl;
			return -1;
		}

		auto json_particles = root["particles"];
		for(int i = 0; i < json_particles.size(); ++i)
		{
			auto member = json_particles[i].getMemberNames();
			auto particle = json_particles[i];
			auto pos = particle["position"];
			auto dir = particle["director"];
			world->AddParticle(
				new Site({pos[0].asFloat(), pos[1].asFloat(), pos[2].asFloat()}, 
				     	 {dir[0].asFloat(), dir[1].asFloat(), dir[2].asFloat()}, 
						 member[0]) 
			);
		}


		delete world;
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;


	}

	return 0;
}