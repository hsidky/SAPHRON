#include "../src/Ensembles/EXEDOSEnsemble.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SpeciesSwapMove.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;

// Testing default EXEDOS Ensemble behavior.
TEST(EXEDOSEnsemble, DefaultBehavior)
{
	/*
	   int n = 10;
	   Models::LebwohlLasherModel model(n, n, n);
	   Moves::SphereUnitVectorMove move;
	   Moves::SpeciesSwapMove move2(2);

	   // Randomize initial positions.a
	   for(int i = 0; i < 3*model.GetSiteCount(); i++)
	   {
	    auto* site = model.DrawSample();
	    move.Perform(*site);
	   }

	   // Random mole fractions
	   model.ConfigureMixture(2, {0.5, 0.5});
	   model.SetInteractionParameter(1.0, 1, 1);
	   model.SetInteractionParameter(sqrt(0.5*1.0), 1, 2);
	   model.SetInteractionParameter(0.5, 2, 2);
	   model.SetIsotropicParameter(0, 1, 1);
	   model.SetIsotropicParameter(0, 1, 2);
	   model.SetIsotropicParameter(0, 2, 2);

	   Ensembles::EXEDOSEnsemble<Site> ensemble(model, 0.1, 0.5, 400, 1.0);
	   ensemble.SetTargetFlatness(0.8);

	   ensemble.AddMove(move);
	   ensemble.AddMove(move2);
	   ensemble.Sweep();
	 */
}
