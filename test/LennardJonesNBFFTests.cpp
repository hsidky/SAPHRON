#include "../src/NewForceFields/LennardJonesNBFF.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(LennardJonesNBFF, InteractionPotential)
{
	auto rcut = 14.;
	LennardJonesNBFF ff(1., 1., {rcut});
	Site s1; 
	Site s2;
	s1.position = {0., 0., 0.};
	s2.position = {1.5, 0., 0.};

	auto r = s1.position - s2.position;
	auto u = ff.Evaluate(s1, s2, r, r.squaredNorm(), 0);
	ASSERT_NEAR(-0.320336594278575, u.energy, 1e-10);
	ASSERT_NEAR(-4.859086276627293e-04, ff.EnergyTailCorrection(0), 1e-10);
	ASSERT_NEAR(-0.002915451636909, ff.PressureTailCorrection(0), 1e-10);

	// Compute pressure from force.
	double P = -1./3.*u.force.dot(r);
	ASSERT_NEAR(-0.579014415, P, 1e-7);
}

TEST(LennardJonesNBFF, ConfigurationValues)
{

}