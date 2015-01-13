#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;

// Default constructor test on NVT Ensemble
TEST(NVTEnsemble, DefaultConstructor)
{
	int n = 37;
	Ising3DModel model(n, 1);
	FlipSpinMove move;
	NVTEnsemble<Site> s(model, 2.5);
	s.AddMove(move);
	for(int i = 0; i < 100; i++)
		s.Sweep();
}
