#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Particles/Site.h"
#include "../src/Constraints/Dynamic1DP2C.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Dynamic1DP2C, DefaultBehavior)
{
	// Initialize particles.
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, "L1");
	Site s2({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, "L2");

	// Initialize constraint. 
	Dynamic1DP2C c(1.0, {0.0, 0.0, 1.0}, 0, {{0.0, 3.0}});

	// Initialize forcefield manager and add constraint.
	ForceFieldManager ffm;
	ffm.AddConstraint("L1", &c);

	// The particle is within the region of interest, pointing in the preferred 
	// direction.
	ASSERT_EQ(-1.0, ffm.EvaluateConstraintEnergy(s1));

	// Try particle S2. Should have no energy. 
	ASSERT_EQ(0.0, ffm.EvaluateConstraintEnergy(s2));

	// Change orientation of s1 and test again.
	s1.SetDirector({0.0, sqrt(2.)/2., sqrt(2.)/2. });
	ASSERT_DOUBLE_EQ(-0.25, ffm.EvaluateConstraintEnergy(s1));

	// Make sure energy comes through during total energy eval. 
	ASSERT_DOUBLE_EQ(-0.25, ffm.EvaluateEnergy(s1).energy.total());

	// Move the particle outside the region. There should be no constraint energy.
	s1.SetPosition({3.01, 0.0, 0.0});
	ASSERT_EQ(0, ffm.EvaluateConstraintEnergy(s1));
}