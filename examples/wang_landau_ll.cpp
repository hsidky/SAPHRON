// This example is a demonstration of Wang-Landau sampling (categorized as an "ensemble")
// of the Lebwohl-Lasher model.

// Include header files.header
#include "../src/Ensembles/WangLandauEnsemble.h"
#include "../src/Loggers/CSVLogger.h"
#include "../src/Loggers/ConsoleLogger.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"

// Include for parsing using stringstream
#include <iostream>
#include <sstream>

// Function definition of our basic input parser.
// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& minE,
                double& maxE,
                int& binCount,
                std::string& modelFile,
                std::string& SitesFile,
                std::string& vecsFile
                );

// The main program expects a user to input the lattice size, number of Wang-Landau
// iterations, minimum and maximum energies, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
int main(int argc, char const* argv[])
{
	int latticeSize, iterations, binCount;
	double minE, maxE;
	std::string modelFile, sitesFile, vecsFile;

	if(argc != 9)
	{
		std::cerr << "Program syntax:" << argv[0] <<
		" lattice-size iterations min-E max-E bin-count model-outputfile site-outputfile DOS-outputfile"
		          << std::endl;
		return 0;
	}

	if(parse_input(argv, latticeSize, iterations, minE, maxE, binCount, modelFile, sitesFile,
	               vecsFile) != 0)
		return -1;

	// Initialize the Lebwohl-Lasher model.
	Models::LebwohlLasherModel model(latticeSize, latticeSize, latticeSize);

	// Initialize the move we would like to perform on the model. This is the basic
	// "unit vector on sphere" move.
	Moves::SphereUnitVectorMove move;

	// Since all descendants of Lattice3D model by default initialize all spins up, we
	// need to randomize the initial positions of the sites (spins). This is critical
	// since the Wang-Landau ensemble rejects moves that occur outside the desired
	// energy binning range. Since our range of interest does not include this configuration,
	// we would be stuck unless we perform this step.
	for(int i = 0; i < 3*model.GetSiteCount(); i++)
	{
		auto* site = model.DrawSample();
		move.Perform(*site);
	}

	// Initialize CSV logger and console logger for output. We want to log our
	// density of states to a CSV file, and just monitor flatness in the console.
	Loggers::CSVLogger csvlogger(modelFile, sitesFile, vecsFile, 5000);
	Loggers::ConsoleLogger consolelogger(1000);

	// Monitor flatness
	auto flatness = [] (BaseModel&, const EnsembleProperty &eprops) {
		return *eprops.at("Flatness");
	};

	// Monitor scale factor
	auto scale = [] (BaseModel&, const EnsembleProperty &eprops) {
		return *eprops.at("ScaleFactor");
	};

	// Log density of states.
	auto dos = [] (BaseModel&, const EnsembleVector &evecs) {
		return *evecs.at("DOS");
	};

	// Register callbacks with loggers.
	csvlogger.AddVectorProperty("DOS", dos);
	consolelogger.AddModelProperty("Flatness", flatness);
	consolelogger.AddModelProperty("ScaleFactor", scale);

	// Initialize Wang-Landau sampler.
	Ensembles::WangLandauEnsemble<Site> ensemble(model, minE, maxE, binCount);

	// Register loggers and moves with the ensemble.
	ensemble.AddLogger(csvlogger);
	ensemble.AddLogger(consolelogger);
	ensemble.AddMove(move);

	// Run WL sampling.
	ensemble.Run(iterations);
}

// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& minE,
                double& maxE,
                int& binCount,
                std::string& modelFile,
                std::string& SitesFile,
                std::string& vecsFile
                )
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
	if(!(ss >> iterations))
	{
		std::cerr << "Invalid iterations. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[3]);
	if(!(ss >> minE))
	{
		std::cerr << "Invalid minimum energy. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[4]);
	if(!(ss >> maxE))
	{
		std::cerr << "Invalid maximum energy. Must be a double." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[5]);
	if(!(ss >> binCount))
	{
		std::cerr << "Invalid bin count. Must be an integer." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[6]);
	if(!(ss >> modelFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[7]);
	if(!(ss >> SitesFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	ss.clear();
	ss.str(args[8]);
	if(!(ss >> vecsFile))
	{
		std::cerr << "Invalid output file. Must be a string." << std::endl;
		return -1;
	}

	return 0;
}
