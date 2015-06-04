#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Particles/Site.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(LennardJonesFF, DefaultBehavior)
{
	LennardJonesFF ff(1.0, 1.0, 14);
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({1.5, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	s1.AddNeighbor(&s2);
	s2.AddNeighbor(&s1);

	ForceFieldManager ffm;
	ffm.AddForceField("L1", "L1", ff);

	std::map<int, int> compositions = 
	{
		{s1.GetSpeciesID(), 1000}
	};

	// Validate invidual components first.
	auto NB = ff.Evaluate(s1, s2);
	ASSERT_NEAR(-0.320336594278575, NB.energy, 1e-10);
	ASSERT_NEAR(-4.859086276627293e-04, ff.EnergyTailCorrection(), 1e-10);
	ASSERT_NEAR(-0.002915451636909, ff.PressureTailCorrection(), 1e-10);
	ASSERT_NEAR(0.772019220697437, NB.virial, 1e-10);

	auto H = ffm.EvaluateHamiltonian(s2, compositions, 30*30*30);

	ASSERT_NEAR(-0.320449670350796, H.energy.nonbonded, 1e-10);
	ASSERT_NEAR(-2.144487494523771e-05, H.pressure.isotropic(), 1e-9);
}
