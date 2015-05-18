// This example is a demonstration of Semi-grand Density of States Sampling
// of a Lebwhol-Lasher binary mixture

// Include header files
// Include for parsing using stringstream
#include <iostream>
#include <sstream>
#include <string>

#include "json/json.h"

// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	Json::Value root;   // 'root' will contain the root value after parsing.
	
	try {
		
		Json::Reader reader;	
		std::cin >> root;
		// Get the value of the member of root named 'encoding',
		// and return 'UTF-8' if there is no such member.
		//std::string encoding = root.get("encoding", "UTF-8" ).asString();
		// Get the value of the member of root named 'plug-ins'; return a 'null' value if
		// there is no such member.
		//const Json::Value plugins = root["plug-ins"];

		// If you like the defaults, you can insert directly into a stream.
		std::cout << root;
		// Of course, you can write to `std::ostringstream` if you prefer.
		// If desired, remember to add a linefeed and flush.
		std::cout << std::endl;
		
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}