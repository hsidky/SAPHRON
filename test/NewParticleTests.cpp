#include "../src/Particles/NewParticle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Particle, DefaultBehavior)
{
	// Test basic methods, getters and setters.
	Site site1, site2;
	site1.position = {1, 1, 2};
	site2.position = {1, 1, 4};

	NewParticle particle(0, {&site1, &site2});

	Vector3 pos{1,1,3};
	ASSERT_EQ(pos, particle.GetPosition());
	ASSERT_EQ(0, particle.GetCharge());
	ASSERT_EQ(2.0, particle.GetMass());

	// Test positions.
	particle.SetPosition(1, {1, 1, 6});
	pos[2] = 4;
	ASSERT_EQ(pos, particle.GetPosition());

	pos = {0, 0, 0};
	particle.SetPosition(pos);
	ASSERT_EQ(pos, particle.GetPosition());
	pos = {0, 0, -2};
	ASSERT_EQ(pos, particle.GetPosition(0));
	pos = {0, 0, 2};
	ASSERT_EQ(pos, particle.GetPosition(1));

	// Test charge calculation.
	particle.SetCharge(0, 2);
	ASSERT_EQ(2, particle.GetCharge());
	ASSERT_EQ(2, particle.GetCharge(0));
	ASSERT_EQ(0, particle.GetCharge(1));

	// Test directors.
	Vector3 dir1 = {0, 0, 1};
	ASSERT_EQ(dir1, particle.GetDirector(0));
	ASSERT_EQ(dir1, particle.GetDirector(1));
	dir1 = {1, 0, 0};
	particle.SetDirector(1, dir1);
	ASSERT_EQ(dir1, particle.GetDirector(1));

	// Test mass.
	particle.SetMass(0, 3);
	particle.SetMass(1, 2);
	ASSERT_EQ(5, particle.GetMass());

	ASSERT_EQ(0, particle.GetSpecies());
	ASSERT_FALSE(particle.IsBonded(0, 0));
	ASSERT_FALSE(particle.IsBonded(0, 1));
	ASSERT_FALSE(particle.IsBonded(1, 1));
	particle.AddBond(0, 1);
	ASSERT_TRUE(particle.IsBonded(0, 1));
	ASSERT_EQ(2, particle.SiteCount());
}