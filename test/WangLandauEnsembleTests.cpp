#include "../src/Ensembles/WangLandauEnsemble.h"
#include "../src/Loggers/ConsoleLogger.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Loggers;

// Testing default WL Ensemble behavior.
TEST(WangLandauEnsemble, DefaultBehavior)
{
	int n = 10;
	Models::LebwohlLasherModel model(n, n, n);
	Moves::SphereUnitVectorMove move;
	Loggers::ConsoleLogger consolelogger(1000);

	// Randomize initial positions.
	for(int i = 0; i < 3*model.GetSiteCount(); i++)
	{
		auto* site = model.DrawSample();
		move.Perform(*site);
	}

	Ensembles::WangLandauEnsemble<Site> ensemble(model, -1.9, -0.5, 10000);

	// Lambda function for logger
	auto flatness = [] (BaseModel &, const EnsembleProperty &ep) {
		return *ep.at("Flatness");
	};

	auto energy = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("Energy");
	};

	auto lo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("LowerOutliers");
	};

	auto uo = [] (BaseModel &, const EnsembleProperty &ep){
		return *ep.at("UpperOutliers");
	};

	// Add magnetization to logger.
	consolelogger.AddModelProperty("Flatness", flatness);
	consolelogger.AddModelProperty("Energy", energy);
	consolelogger.AddModelProperty("LowerOutliers", lo);
	consolelogger.AddModelProperty("UpperOutliers", uo);
	ensemble.AddLogger(consolelogger);
	ensemble.AddMove(move);

	ensemble.Sweep();
}
