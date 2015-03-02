#include "../src/Ensembles/WangLandauDOSEnsemble.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;

// Testing default WL Ensemble behavior.
TEST(WangLandauDOSEnsemble, DefaultBehavior)
{
	int n = 10;
	Models::LebwohlLasherModel model(n, n, n);
	Moves::SphereUnitVectorMove move;

	// Randomize initial positions.
	for(int i = 0; i < 3*model.GetSiteCount(); i++)
	{
		auto* site = model.DrawSample();
		move.Perform(*site);
	}

	Ensembles::WangLandauDOSEnsemble<Site> ensemble(model, -1.9, -0.5, 10000);
	ensemble.SetTargetFlatness(0.5);
	ensemble.AddMove(move);
	ensemble.Sweep();
}
