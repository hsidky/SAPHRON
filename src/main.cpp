#include "Ensembles/NVTEnsemble.h"
#include "Loggers/CSVLogger.h"
#include "Models/Ising3DModel.h"
#include "Moves/FlipSpinMove.h"
#include <iostream>
#include <sstream>

using namespace std;

int main (int argc, char const* argv[])
{
	if(argc != 4)
	{
		cerr << "Program syntax:" << argv[0] <<
		" lattice-size temperature model-outputfile site-outputfile" << endl;
		return 0;
	}

	int latticeSize;
	double temperature;
	string outputModel, outputSites;
	stringstream ss;

	// Parse input
	ss.clear();
	ss.str(argv[1]);
	if(!(ss >> latticeSize))
		cerr << "Invalid lattice size. Must be an integer." << endl;

	ss.clear();
	ss.str(argv[2]);
	if(!(ss >> temperature))
		cerr << "Invalid temperature. Must be an double." << endl;

	ss.clear();
	ss.str(argv[3]);
	if(!(ss >> outputModel))
		cerr << "Invalid output file. Must be an string." << endl;

	ss.clear();
	ss.str(argv[4]);
	if(!(ss >> outputSites))
		cerr << "Invalid output file. Must be an string." << endl;

	// Initialize model.
	Models::Ising3DModel model(latticeSize);
	Moves::FlipSpinMove move;
	Loggers::CSVLogger logger(outputModel, outputSites);

	// Add loggable data
	// Coordinates

	// Write headers

	return 0;
}
