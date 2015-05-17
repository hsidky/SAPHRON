// This example is a demonstration of Semi-grand Density of States Sampling
// of a Lebwhol-Lasher binary mixture

// Include header files
// Include for parsing using stringstream
#include <iostream>
#include <sstream>
#include <locale.h>
#include <string>
#include "config4cpp/Configuration.h"
#include "config4cpp/SchemaValidator.h"

using namespace config4cpp;	

// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	setlocale(LC_ALL, "");
	SchemaValidator * sv = new SchemaValidator();
	const char * ensemble_schema [] = {
		"@required type = enum[NVT, DOS]",
		"@required sweeps = int[0,1000000000]",
		"@optional temperature = float_with_units[K, C, F, RU]",
		"@optional seed = int",
		0
	};

	const char * dos_schema [] = {
		"@required type = enum[WangLandau, ElasticCoeff]",
		"@required interval = tuple[float,min, float,max]",
		"@optional order_parameter = scope",
		"@optional bin_count = int[0,1000000]",
		"@optional bin_width = float[0,10000000]",
		"@optional scale_factor = float[0,10000000]",
		"@optional target_flatness = float[0,1]",
		0
	};

	const char * dos_op_elastic_schema [] = {
		"@required dxj = float",
		0
	};

	const char * scope_ensmeble = "ensemble";
	const char * scope_dos = "DOS";
	const char * scope_dos_op = "DOS.order_parameter";
	Configuration * cfg = Configuration::create();
	try
	{
		// Parse file and schema. 
		cfg->parse("foo.cfg");
		sv->parseSchema(ensemble_schema);
		sv->validate(cfg, scope_ensmeble, "");
		const char * ensemble_type = cfg->lookupString(scope_ensmeble, "type");
		if(strncmp(ensemble_type, "DOS", 3) == 0)
		{
			sv->parseSchema(dos_schema);
			sv->validate(cfg, scope_dos, "", false, Configuration::Type::CFG_SCOPE_AND_VARS);
			
			const char * order_parameter = cfg->lookupString(scope_dos, "type");
			if(strncmp(order_parameter, "ElasticCoeff", 12) == 0)
			{
				sv->parseSchema(dos_op_elastic_schema);
				sv->validate(cfg, scope_dos_op, "");
			}
		}

	} 
	catch(const ConfigurationException & ex) {
		std::cout << ex.c_str() << std::endl;
	}
	cfg->destroy();
	return 0;
}