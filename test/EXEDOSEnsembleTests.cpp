#include "../src/Ensembles/EXEDOSEnsemble.h"
#include "../src/Loggers/ConsoleLogger.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SpeciesSwapMove.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Loggers;

// Testing default EXEDOS Ensemble behavior.
TEST(EXEDOSEnsemble, DefaultBehavior)
{
	int n = 10;
	Models::LebwohlLasherModel model(n, n, n);
	Moves::SphereUnitVectorMove move;
	Moves::SpeciesSwapMove move2(2);
	Loggers::ConsoleLogger consolelogger(1000);

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

	Ensembles::EXEDOSEnsemble<Site> ensemble(model, 1.0, 0.1, 0.5, 400);
	ensemble.SetTargetFlatness(0.8);

	// Lambda function for logger
	auto flatness = [] (BaseModel &, const EnsembleProperty &ep) {
		return *ep.at("Flatness");
	};

	auto dos = [] (BaseModel &, const EnsembleVector &ep) {
		return *ep.at("DOS");
	};

	auto n1count = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("SpeciesCount");
	};

	auto lo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("LowerOutliers");
	};

	auto uo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("UpperOutliers");
	};

	consolelogger.AddModelProperty("Flatness", flatness);
	consolelogger.AddModelProperty("SpeciesCount", n1count);
	consolelogger.AddModelProperty("LowerOutliers", lo);
	consolelogger.AddModelProperty("UpperOutliers", uo);
	consolelogger.AddVectorProperty("DOS", dos);
	//ensemble.AddLogger(consolelogger);

	ensemble.AddMove(move);
	ensemble.AddMove(move2);
	ensemble.Sweep();
}
