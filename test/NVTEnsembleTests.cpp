#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "../src/Visitors/ConsoleVisitor.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Visitors;

// Expected magnetization for the NVT Ensemble on the 3D Ising model at various temperatures.
TEST(NVTEnsemble, IsingModelMagnetization)
{
	int n = 10;
	Ising3DModel model(n, n, n, 1);
	FlipSpinMove move;
	NVTEnsemble<Site> s(model, 1.0);

	SimFlags flags;

	flags.temperature = 1;
	flags.iterations = 1;
	flags.energy = 1;
	flags.site_coordinates = 1;
	ConsoleVisitor c(flags, 100);

	s.AddMove(move);
	s.AddObserver(&c);

	// Iterate
	for(int i = 0; i < 100; i++)
		s.Sweep();

	// Change temperature
	s.SetTemperature(5.0);

	// Iterate
	for(int i = 0; i < 100; i++)
		s.Sweep();
}
