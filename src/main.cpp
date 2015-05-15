// This example is a demonstration of Semi-grand Density of States Sampling
// of a Lebwhol-Lasher binary mixture

// Include header files
// Include for parsing using stringstream
#include <iostream>
#include <sstream>
#include <locale.h>
#include "config4cpp/Configuration.h"

using namespace config4cpp;

// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	const char *    logDir;
	int             logLevel, timeout;
	const char *    scope = "foo_srv";
	Configuration * cfg = Configuration::create();
	
	try
	{
		cfg->parse("foo.cfg");
	  	logDir   = cfg->lookupString(scope, "log.dir");
	  	logLevel = cfg->lookupInt(scope, "log.level");
	  	timeout  = cfg->lookupDurationSeconds(scope, "timeout");
	} 
	catch(const ConfigurationException & ex) {
		std::cout << ex.c_str() << std::endl;
	}
	cfg->destroy();
	return 0;
}