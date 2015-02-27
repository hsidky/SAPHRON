// This example is a demonstration of the Lebwohl-Lasher model in an NVT ensemble
// and basic usage of the API. It has a very basic command line interface to
// illustrate a possible use case.

// Include the pertinent header files.
#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"

// Includes for parsing using stringstream.
#include <iostream>
#include <sstream>

// Function definition of our basic input parser.
// // A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& temperature,
                std::string& modelFile,
                std::string& SitesFile);

// The main program expects a user to input the lattice size, reduced temperature,
// number of iterations and specify an output file (csv) for model properties and
// site properties.
int main(int argc, char const* argv[])
{
	int latticeSize, iterations;
	double temperature;
	std::string modelFile, sitesFile;

	// We expect a certain number of arugments.
	if(argc != 6)
	{
		std::cerr << "Program syntax:" << argv[0] <<
		" lattice-size temperature iterations model-outputfile site-outputfile" <<
		std::endl;
		return 0;
	}

	// Exit on parsing error.
	if(parse_input(argv, latticeSize, iterations, temperature, modelFile, sitesFile) != 0)
		return -1;

	// Initialize the Lebwohl-Lasher lattice model. The constructor takes in one
	// required argument which is the lattice size - the length of one dimension.
	// The model initializes the required number of sites in the appropriate locations,
	// defines the nearest-neighbors and provides an initial spin.
	Models::LebwohlLasherModel model(latticeSize, latticeSize, latticeSize);

	// Initialize the NVT ensemble. For this particular ensemble, it requires a reference to
	// the model and the temperature. The template parametr for the ensemble is the type
	// returned by the DrawSample method of the model - in this case it is a site.
	//Ensembles::NVTEnsemble<Site> ensemble(model, temperature);

	// Initialize the moves. These are the different Monte-Carlo moves we would like
	// to perform on the model above. In this case, we want to pick a random unit
	// vector on a sphere. A "Move" class operates on a site within model in an ensemble.
	Moves::SphereUnitVectorMove move;

	//ensemble.AddMove(move);

	// Sweep for iterations
	//for (int i = 0; i < iterations; i++)
		//ensemble.Sweep();

	return 0;
}

// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& temperature,
                std::string& modelFile,
                std::string& SitesFile)
{
	std::stringstream ss;

	// Parse input
	ss.clear();
	ss.str(args[1]);
	if(!(ss >> latticeSize))
	{
		std::cerr << "Invalid lattice size. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[2]);
	if(!(ss >> temperature))
	{
		std::cerr << "Invalid temperature. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[3]);
	if(!(ss >> iterations))
	{
		std::cerr << "Invalid iterations. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[4]);
	if(!(ss >> modelFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[5]);
	if(!(ss >> SitesFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	return 0;
}
