// This example is a demonstration of the Lebwohl-Lasher model in an NVT ensemble
// confined within a cell with anchoring on the YZ plane (x=0, x=L).

// Include the pertinent header files.
#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/DataLoggers/CSVDataLogger.h"
#include "../src/DataLoggers/ConsoleDataLogger.h"
#include "../src/Models/LLCellModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"

// Includes for parsing using stringstream.
#include <iostream>
#include <sstream>

// Function definition of our basic input parser.
// A very basic input parser.
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

	// Define the anchored vectors on either end of the cell.
	std::vector<double> vBegin {1.0, 0.0, 0.0};
	std::vector<double> vEnd {0.0, 1.0, 0.0};

	// Initialize the Lebwohl-Lasher cell lattice model.
	Models::LLCellModel model(latticeSize, latticeSize, latticeSize, vBegin, vEnd);

	// Initialize the NVT ensemble. For this particular ensemble, it requires a reference to
	// the model and the temperature. The template parametr for the ensemble is the type
	// returned by the DrawSample method of the model - in this case it is a site.
	Ensembles::NVTEnsemble<Site> ensemble(model, temperature);

	// Initialize the moves. These are the different Monte-Carlo moves we would like
	// to perform on the model above. In this case, we want to pick a random unit
	// vector on a sphere. A "Move" class operates on a site within model in an ensemble.
	Moves::SphereUnitVectorMove move;

	// The CSV logger constructor requires that we provide file names for output.
	// Note: vector file not used, so we input placeholder.
	DataLoggers::CSVDataLogger csvlogger(modelFile, sitesFile, "vecs.csv", 1000);
	DataLoggers::ConsoleDataLogger consolelogger(1000);

	// Log total energy.
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

	// For the CSV logger, we want to log the site locations and unit vectors.
	csvlogger.AddSiteProperty("z",
	                          [] (Site & site,
	                              const EnsembleProperty &){ return site.GetZCoordinate();
				  });
	csvlogger.AddSiteProperty("y",
	                          [] (Site& site,
	                              const EnsembleProperty &){ return site.GetYCoordinate();
				  });
	csvlogger.AddSiteProperty("x",
	                          [] (Site& site,
	                              const EnsembleProperty &){ return site.GetXCoordinate();
				  });
	csvlogger.AddSiteProperty("uz",
	                          [] (Site & site,
	                              const EnsembleProperty &){ return site.GetZUnitVector();
				  });
	csvlogger.AddSiteProperty("uy",
	                          [] (Site& site,
	                              const EnsembleProperty &){ return site.GetYUnitVector();
				  });
	csvlogger.AddSiteProperty("ux",
	                          [] (Site& site,
	                              const EnsembleProperty &){ return site.GetXUnitVector();
				  });

	// Finally, we add both the loggers and the move to the ensemble.
	ensemble.AddLogger(csvlogger);
	ensemble.AddLogger(consolelogger);
	ensemble.AddMove(move);

	/*
	   for(int i = 0; i < 3*model.GetSiteCount(); i++)
	   {
	    auto* site = model.DrawSample();
	    move.Perform(*site);
	   }
	 */

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
