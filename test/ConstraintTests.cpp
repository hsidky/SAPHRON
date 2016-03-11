#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Particles/Particle.h"
#include "../src/Constraints/DirectorRestrictionC.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(DirectorRestrictionC, DefaultBehavior)
{
	// Initialize particles and world.
	auto l = 10.;
	World world(l, l, l, 1.0, 1.0);
	Particle* site1 = new Particle({l/2., 0.0, 0.0}, {0.0, 0.0, 1.0}, "E1");
	Particle* site2 = new Particle({l/2., 0.0, 10.0}, {0.0, 0.0, 1.0}, "E1");
	world.AddParticle(site1);
	world.AddParticle(site2);

	ASSERT_EQ(2, world.GetParticleCount());

	// Initialize constraint.
	DirectorRestrictionC c(&world, 1.0, {0.0, 0.0, 1.0}, 0, {{4.0, 6.0}});

	// Initialize forcefield manager and add constraint.
	ForceFieldManager ffm;
	ffm.AddConstraint(world, &c);

	ASSERT_EQ(-1.0, ffm.EvaluateConstraintEnergy(world));

	site1->SetDirector({0.0, 1.0, 0.0});
	site2->SetDirector({0.0, 1.0, 0.0});

	ASSERT_EQ(0.50, ffm.EvaluateConstraintEnergy(world));

	// Move site 2 outside, change its director. Only site1 should be represented.
	site2->SetPosition({7.0, 0.0, 0.0});
	site2->SetDirector({0.0, 0.0, 1.0});

	ASSERT_EQ(0.50, ffm.EvaluateConstraintEnergy(world));
}