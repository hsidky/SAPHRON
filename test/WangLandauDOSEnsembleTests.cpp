#include "../src/Ensembles/WangLandauDOSEnsemble.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "../src/Visitors/ConsoleVisitor.h"
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

	SimFlags flags;
	flags.energy = 1;
	flags.dos_walker = 1;
	flags.dos_flatness = 1;
	flags.dos_interval = 1;
	flags.dos_scale_factor = 1;
	flags.hist_bin_count = 1;
	flags.hist_lower_outliers = 1;
	flags.hist_upper_outliers = 1;

	Visitors::ConsoleVisitor console(flags, 1000);
	Ensembles::WangLandauDOSEnsemble<Site> ensemble(model, -1.9, -0.5, 10000);
	ensemble.AddObserver(&console);
	ensemble.SetTargetFlatness(0.5);
	ensemble.AddMove(move);
	ensemble.Sweep();
}
