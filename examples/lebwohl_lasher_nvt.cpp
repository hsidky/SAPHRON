// This example is a demonstration of the Lebwohl-Lasher model in an NVT ensemble
// and basic usage of the API. It has a very basic command line interface to
// illustrate a possible use case.

// Include the pertinent header files.
#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/DataLoggers/CSVDataLogger.h"
#include "../src/DataLoggers/ConsoleDataLogger.h"
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
	Ensembles::NVTEnsemble<Site> ensemble(model, temperature);

	// Initialize the moves. These are the different Monte-Carlo moves we would like
	// to perform on the model above. In this case, we want to pick a random unit
	// vector on a sphere. A "Move" class operates on a site within model in an ensemble.
	Moves::SphereUnitVectorMove move;

	// Initialize loggers. The loggers are responsible for recording the desired property
	// data from a simulation. It is possible to have many loggers operate on a single
	// simulation. In this case, we will record energy to a CSV file every 10 iterations,
	// and use the console logger to display the energy output every 100.

	// The CSV logger constructor requires that we provide file names for output.
	DataLoggers::CSVDataLogger csvlogger(modelFile, sitesFile, "vecs.csv", 10);
	DataLoggers::ConsoleDataLogger consolelogger(100);

	// There are two types of properties that can be logged: model properties and site properties.
	// Model properties are provided with a reference to the model object which allows them to
	// perform calculations at the model level, an example of this would be
	// total energy. Site properties are associated with individual sites, such as
	// position, velocity or orientation. "Ensemble properties" are passed by reference to
	// both types of properties. These are std::maps that are implemented by each Ensemble
	// and must be checked accordingly.

	// Create a callback to calculate the total energy of the system. Here we use a
	// Lambda function that we can re-use for both loggers. Since we really are not using
	// EnsembleProperty, we leave that undeclared. Ensembles would typically track energy
	// internally, and could potentially be accessed via EnsembleProperty. This example is
	// for illustrative purposes.
	auto energy = [] (BaseModel& model, const EnsembleProperty &) {
		double u = 0;
		int c = model.GetSiteCount();
		for(int i = 0; i < c; i++)
			u += model.EvaluateHamiltonian(i);
		u /= 2.0*c;
		return u;
	};

	// Attach the callback to both loggers. We should provide a name.
	csvlogger.AddModelProperty("Energy", energy);
	consolelogger.AddModelProperty("Energy", energy);

	// Finally, we add both the loggers and the move to the ensemble.
	ensemble.AddLogger(csvlogger);
	ensemble.AddLogger(consolelogger);
	ensemble.AddMove(move);

	// The CSV logger provides us a method that allows us to write headers to
	// the CSV file if we please.
	csvlogger.WriteHeaders();

	// Sweep for iterations
	for (int i = 0; i < iterations; i++)
		ensemble.Sweep();

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
