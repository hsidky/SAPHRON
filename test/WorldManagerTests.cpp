#include "../src/Worlds/WorldManager.h"
#include "../src/Worlds/World.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(WorldManager, DefaultBehavior)
{
	// Create dummy worlds.
	World w1(10.0, 10.0, 10.0, 1.0);
	World w2(10.0, 10.0, 10.0, 1.0);

	// Initialize WorldManager.
	WorldManager sim;

	// Add one world and check defaults.
	sim.AddWorld(&w1);
	ASSERT_EQ(0, sim.GetWorld()->GetTemperature());
	ASSERT_EQ(0, sim.GetWorld(0)->GetTemperature());

	// Change temperature.
	w1.SetTemperature(3.0);
	ASSERT_EQ(3.0, sim.GetWorld()->GetTemperature());

	// Add another world. 
	sim.AddWorld(&w2);
	ASSERT_EQ(2, sim.GetWorldCount());

	// Make sure adding same worlds does nothing.
	sim.AddWorld(&w1);
	sim.AddWorld(&w2);
	ASSERT_EQ(2, sim.GetWorldCount());

	// Change active world.
	sim.SetActiveWorld(&w2);
	ASSERT_EQ(&w2, sim.GetWorld());
	sim.SetActiveWorld(&w1);
	ASSERT_EQ(&w1, sim.GetWorld());

	// Remove world and check. 
	sim.RemoveWorld(&w1);
	ASSERT_EQ(1, sim.GetWorldCount());
	ASSERT_EQ(&w2, sim.GetWorld());
}