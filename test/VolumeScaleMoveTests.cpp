#include "../src/Moves/VolumeScaleMove.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"
#include <cmath>

using namespace SAPHRON;

TEST(VolumeScaleMove, DefaultBehavior)
{
	double l1 = 30.0;
	double l2 = 15.0;
	double v1 = l1*l1*l1;
	double v2 = l2*l2*l2;

	SimpleWorld world1(l1, l1, l1, 1.0);
	SimpleWorld world2(l2, l2, l2, 1.0);

	ASSERT_EQ(v1, world1.GetVolume());
	ASSERT_EQ(v2, world2.GetVolume());

	VolumeScaleMove move(2.0);

	// Test move on one world.
	ParticleList pl{};
	move.Perform(world1, pl);
	ASSERT_LE(world1.GetVolume()/v1 , exp(2.0));
	ASSERT_GE(world1.GetVolume()/v1 , exp(-2.0));

	move.Undo();
	ASSERT_DOUBLE_EQ(v1, world1.GetVolume());

	// Test move on both worlds. 
	WorldIndexList windex = {0, 1};
	move.Perform({&world1, &world2}, windex, pl);
	ASSERT_DOUBLE_EQ(v1 + v2, world1.GetVolume() + world2.GetVolume());
	ASSERT_LE(world1.GetVolume()/v1 , exp(2.0));
	ASSERT_GE(world1.GetVolume()/v1 , exp(-2.0));
	ASSERT_LE(world2.GetVolume()/v2 , exp(2.0));
	ASSERT_GE(world2.GetVolume()/v2 , exp(-2.0));

	move.Undo();
	ASSERT_DOUBLE_EQ(v1, world1.GetVolume());
	ASSERT_DOUBLE_EQ(v2, world2.GetVolume());
}
