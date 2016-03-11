// This example is a demonstration of the Lebwohl-Lasher model in an NVT ensemble
// and basic usage of the API. It has a very basic command line interface to
// illustrate a possible use case.

// Include the pertinent header files.
#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Particles/Particle.h"
#include "../src/Observers/CSVObserver.h"
#include "../src/Observers/ConsoleObserver.h"
#include "../src/Worlds/SimpleWorld.h"

// Includes for parsing using stringstream.
#include <iostream>
#include <sstream>

using namespace SAPHRON;

// Function definition of our basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& temperature,
                std::string& filePrefix);

// The main program expects a user to input the lattice size, reduced temperature,
// number of iterations and specify an output file prefix (csv) for logged properties.
int main(int argc, char const* argv[])
{
	int latticeSize, iterations;
	double temperature;
	std::string filePrefix;

	// We expect a certain number of arugments.
	if(argc != 5)
	{
		std::cerr << "Program syntax:" << argv[0] <<
		" lattice-size temperature iterations outputfile-prefix" <<
		std::endl;
		return 0;
	}

	// Exit on parsing error.
	if(parse_input(argv, latticeSize, iterations, temperature, filePrefix) != 0)
		return -1;

	// Initialize a simple cubic lattice "world". This defines the type of box for our simulation.
	// The constructor takes in the lattice dimensions and an optional seed for the pseudo random 
	// number generator.
	SimpleWorld world(latticeSize, latticeSize, latticeSize, 1);

	// Initialize a site. This will represent an individual spin on the lattice in our world. 
	// The constructor takes in an initial position, director and species type (string). Note that 
	// the position is irrelevant since this site will be copied and configured appropriately for 
	// the system.
	Particle site1({ 0, 0, 0 }, { 1.0, 0, 0 }, "E1");

	// Now we configure the world but providing a reference to our spin and telling it to fill the 
	// box entirely with that site. We then configure connectivity through neighbor list 
	// initialization.
	world.ConfigureParticles({ &site1 }, { 1.0 });
	world.UpdateNeighborList();


	// Initialize the Lebwohl-Lasher (LL) force field and add it to an instance of the forcefield
	// manager. The LL forcefield takes in two arguments. An anisotropic interaction energy and 
	// an isotropic term as well. See header file for details.
	LebwohlLasherFF ff(1.0, 0);
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("E1", "E1", ff);

	// Initialize the move we want to perform on our sites, then add it to an instance of the 
	// move manager. The only argument the constructor takes is a seed for the PRNG.
	SphereUnitVectorMove move1(33);
	MoveManager mm;
	mm.PushMove(move1);

	// Initialize the "observers". These are effectively the loggers that will be monitoring the 
	// simulation. We want to print out some information to the console to monitor the run, 
	// but store more details in a CSV file format.
	SimFlags flags1;
	flags1.temperature = 1;
	flags1.iterations = 1;
	flags1.energy = 1;
	flags1.acceptance = 1;
	ConsoleObserver console(flags1, 1000);

	SimFlags flags2;
	flags2.temperature = 1;
	flags2.iterations = 1;
	flags2.energy = 1;
	flags2.acceptance = 1;
	CSVObserver csv(filePrefix, flags2, 500);
        
	// Initialize the NVT ensemble. For this particular ensemble, it requires a reference to world, 
	// the forcefield manager and the move manager. It also takes in a temperature and RNG seed.
	NVTEnsemble ensemble(world, ffm, mm, temperature, 45);

	// Add the observers to the ensemble. 
	ensemble.AddObserver(&console);
	ensemble.AddObserver(&csv);	

	// Run the simulation. 
	ensemble.Run(iterations);

	return 0;
}

// A very basic input parser.
int parse_input(char const* args[], int& latticeSize,
                int& iterations,
                double& temperature,
                std::string& filePrefix)
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
	if(!(ss >> filePrefix))
	{
		std::cerr << "Invalid output file prefix. Must be a string." << std::endl;
		return -1;
	}

	return 0;
}
