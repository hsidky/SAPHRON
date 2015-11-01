#include "../src/Moves/VolumeSwapMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Worlds/World.h"
#include "gtest/gtest.h"
#include <cmath>

using namespace SAPHRON;

TEST(VolumeSwapMove, DefaultBehavior)
{
	double l1 = 30.0;
	double l2 = 15.0;
	double v1 = l1*l1*l1;
	double v2 = l2*l2*l2;

	World world1(l1, l1, l1, 1.0);
	World world2(l2, l2, l2, 1.0);

	ASSERT_EQ(v1, world1.GetVolume());
	ASSERT_EQ(v2, world2.GetVolume());

	ForceFieldManager ffm;
	WorldManager wm; 
	wm.AddWorld(&world1);
	wm.AddWorld(&world2);

	VolumeSwapMove move(2.0);

	// Swap volume with force undo. Make sure everything remains the same. 
	move.Perform(&wm, &ffm, MoveOverride::ForceReject);

	ASSERT_DOUBLE_EQ(v1, world1.GetVolume());
	ASSERT_DOUBLE_EQ(v2, world2.GetVolume());

	// Swap volume and ensure it respects bounds.
	move.Perform(&world1, &world2);
	ASSERT_NEAR(v1 + v2, world1.GetVolume() + world2.GetVolume(), 1e-10);
	ASSERT_LE(world1.GetVolume()/v1 , exp(2.0));
	ASSERT_GE(world1.GetVolume()/v1 , exp(-2.0));
	ASSERT_LE(world2.GetVolume()/v2 , exp(2.0));
	ASSERT_GE(world2.GetVolume()/v2 , exp(-2.0));
}
