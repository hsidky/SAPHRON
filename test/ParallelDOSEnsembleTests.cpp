#include "../src/Ensembles/ParallelDOSEnsemble.h"
#include "../src/Ensembles/WangLandauDOSEnsemble.h"
#include "../src/Models/LebwohlLasherModel.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "gtest/gtest.h"

using namespace Ensembles;

// Testing the default constructor on Parallel DOS.
TEST(ParallelDOSEnsemble, DefaultConstructor)
{
	int n = 10;
	Models::LebwohlLasherModel model(n, n, n);

	ParallelDOSEnsemble<Site, WangLandauDOSEnsemble<Site> >
	ensemble(model, -1.9, -0.5, 10000, 5, 0.5);

	// Check intervals
	std::vector<Interval> intervals {
		{-1.9,                -1.4333333333333333},
		{-5.0/3.0,            -1.2},
		{-1.4333333333333333, -0.9666666666666666},
		{-1.20,               -0.7333333333333333},
		{-0.9666666666666666, -0.50}
	};

	// Validate intervals.
	double nt = pow(n,3);
	for(int i = 0; i < (int)intervals.size(); i++)
	{
		auto ci = ensemble.GetWalkerInterval(i+1);
		ASSERT_DOUBLE_EQ(intervals[i].first*nt, ci.first);
		ASSERT_DOUBLE_EQ(intervals[i].second*nt, ci.second);
	}

	// Validate intervals in each model.
	for(int i = 0; i < (int)intervals.size(); i++)
	{
		auto intv = ensemble.GetEnsemble(i+1)->GetParameterInterval();
		ASSERT_DOUBLE_EQ(intervals[i].first*nt, intv.first);
		ASSERT_DOUBLE_EQ(intervals[i].second*nt, intv.second);
	}

	// Validate walker IDs.
	for(int i = 0; i < (int)intervals.size(); i++)
	{
		auto e = ensemble.GetEnsemble(i+1);
		ASSERT_EQ(i, e->GetWalkerID());
	}
}

// Test parallel behavior.
TEST(ParallelDOSEnsemble, ParallelBehavior)
{
	int n = 10;
	Models::LebwohlLasherModel model(n, n, n);
	Moves::SphereUnitVectorMove move;

	ParallelDOSEnsemble<Site, WangLandauDOSEnsemble<Site> >
	ensemble(model, -1.9, -0.5, 2000, 4, 0.5);

	ensemble.AddMove(move);
	ensemble.Sweep();
}
