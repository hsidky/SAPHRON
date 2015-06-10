// // This example is a demonstration of Semi-grand Density of States Sampling
// // of a Lebwhol-Lasher binary mixture

// // Include header files
// #include "../src/Ensembles/ParallelDOSEnsemble.h"
// #include "../src/Ensembles/SemiGrandDOSEnsemble.h"
// #include "../src/Models/LebwohlLasherModel.h"
// #include "../src/Moves/SpeciesSwapMove.h"
// #include "../src/Moves/SphereUnitVectorMove.h"
// #include "../src/Observers/CSVObserver.h"
// #include "../src/Observers/ConsoleObserver.h"

// // Include for parsing using stringstream
// #include <iostream>
// #include <sstream>
// #include <thread>

// using namespace Ensembles;

// // Function definition of our basic input parser.
// // A very basic input parser.
// int parse_input(char const* args[], int& latticeSize,
//                 double& temperature,
//                 int& iterations,
//                 double& minX,
//                 double& maxX,
//                 double& binWidth,
//                 std::string& prefix
//                 );

// // The main program expects a user to input the lattice size, number of EXEDOS
// // iterations, minimum and maximum mole fractions, number of bins for density-of-states
// // histogram and model, sites and vector file outputs.
// int main(int argc, char const* argv[])
// {
// 	int latticeSize, iterations;
// 	double minX, maxX, temperature, binWidth;
// 	std::string prefix;

// 	if(argc != 8)
// 	{
// 		std::cerr << "Program syntax:" << argv[0] <<
// 		" lattice-size temperature iterations min-X max-X bin-width output-file-prefix"
// 		          << std::endl;
// 		return 0;
// 	}

// 	if(parse_input(argv, latticeSize, temperature, iterations, minX, maxX, binWidth, prefix) != 0)
// 		return -1;

// 	// Initialize the Lebwohl-Lasher model.
// 	Models::LebwohlLasherModel model(latticeSize, latticeSize, latticeSize);

// 	// Initialize the moves we would like to perform on the model. This is the basic
// 	// "unit vector on sphere" move and a species swap move.
// 	Moves::SphereUnitVectorMove move1;
// 	Moves::SpeciesSwapMove move2(2);

// 	// Since all descendants of Lattice3D model by default initialize all spins up, we
// 	// need to randomize the initial configurations of the sites (spins).
// 	for(int i = 0; i < 3*model.GetSiteCount(); i++)
// 	{
// 		auto* site = model.DrawSample();
// 		move1.Perform(*site);
// 	}

// 	// Define interaction parameters
// 	double gaa = 1.0;
// 	double gbb = 1.0;
// 	double ebb = 1.0;
// 	double eaa = 0.4;

// 	// Anisotropic interactions.
// 	model.SetInteractionParameter(eaa, 1, 1);
// 	model.SetInteractionParameter(sqrt(eaa*ebb), 1, 2);
// 	model.SetInteractionParameter(ebb, 2, 2);

// 	// Isotropic interactions.
// 	model.SetIsotropicParameter(gaa, 1, 1);
// 	model.SetIsotropicParameter(sqrt(gaa*gbb), 1, 2);
// 	model.SetIsotropicParameter(gbb, 2, 2);

// 	// Add CSV logger and setup parameters we want to record.
// 	Simulation::SimFlags flags;
// 	flags.identifier = 1;
// 	flags.iterations = 1;
// 	flags.dos_flatness = 1;
// 	flags.dos_values = 1;
// 	flags.dos_scale_factor = 1;
// 	flags.dos_interval = 1;
// 	flags.hist_values = 1;
// 	flags.hist_bin_count = 1;
// 	Simulation::CSVObserver csvobserver(prefix,flags,10000);

// 	Simulation::SimFlags flags2;
// 	flags2.identifier = 1;
// 	flags2.iterations = 1;
// 	flags2.dos_flatness = 1;
// 	flags2.dos_scale_factor = 1;
// 	flags2.hist_bin_count = 1;
// 	flags2.hist_lower_outliers = 1;
// 	flags2.hist_upper_outliers = 1;
// 	flags2.dos_interval = 1;
// 	Simulation::ConsoleObserver consoleobserver(flags2,1000);

// 	// Get number of threads.
// 	int n = std::thread::hardware_concurrency();

// 	ParallelDOSEnsemble<Site, SemiGrandDOSEnsemble<Site> >
// 	ensemble(model, minX, maxX, binWidth, n, 0.5, temperature);

// 	// Register loggers and moves with the ensemble.
// 	ensemble.AddMove(move1);
// 	ensemble.AddMove(move2);
// 	ensemble.AddObserver(&csvobserver);
// 	ensemble.AddObserver(&consoleobserver);

// 	//Run sampling.
// 	ensemble.SetTargetFlatness(0.85);
// 	ensemble.Run(iterations);
// }

// // A very basic input parser.
// int parse_input(char const* args[], int& latticeSize,
//                 double& temperature,
//                 int& iterations,
//                 double& minE,
//                 double& maxE,
//                 double& binWidth,
//                 std::string& prefix
//                 )
// {
// 	std::stringstream ss;

// 	// Parse input
// 	ss.clear();
// 	ss.str(args[1]);
// 	if(!(ss >> latticeSize))
// 	{
// 		std::cerr << "Invalid lattice size. Must be an integer." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[2]);
// 	if(!(ss >> temperature))
// 	{
// 		std::cerr << "Invalid temperature. Must be a double." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[3]);
// 	if(!(ss >> iterations))
// 	{
// 		std::cerr << "Invalid iterations. Must be an integer." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[4]);
// 	if(!(ss >> minE))
// 	{
// 		std::cerr << "Invalid minimum energy. Must be a double." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[5]);
// 	if(!(ss >> maxE))
// 	{
// 		std::cerr << "Invalid maximum energy. Must be a double." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[6]);
// 	if(!(ss >> binWidth))
// 	{
// 		std::cerr << "Invalid bin width. Must be an double." << std::endl;
// 		return -1;
// 	}

// 	ss.clear();
// 	ss.str(args[7]);
// 	if(!(ss >> prefix))
// 	{
// 		std::cerr << "Invalid output file. Must be a string." << std::endl;
// 		return -1;
// 	}

// 	return 0;
// }
