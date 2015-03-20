#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Visitors;

// Expected magnetization for the NVT Ensemble on the 3D Ising model at various temperatures.
TEST(NVTEnsemble, DefaultBehavior)
{
	int n = 10;
	Ising3DModel model(n, n, n, 1);
	FlipSpinMove move;
	NVTEnsemble<Site> s(model, 1.0);

	s.AddMove(move);
	// Iterate
	for(int i = 0; i < 100; i++)
		s.Sweep();

	ASSERT_EQ(1.0, s.GetTemperature());

	// Change temperature
	s.SetTemperature(5.0);
	ASSERT_EQ(5.0, s.GetTemperature());

	// Iterate
	for(int i = 0; i < 100; i++)
		s.Sweep();
}
