#include "../src/Simulation/Simulation.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(Simulation, DefaultBehavior)
{
	// Create dummy worlds.
	SimpleWorld w1(10.0, 10.0, 10.0, 1.0);
	SimpleWorld w2(10.0, 10.0, 10.0, 1.0);

	// Initialize simulation.
	Simulation sim;

	// Add one world and check defaults.
	sim.AddWorld(&w1);
	ASSERT_EQ(0, sim.GetWorld()->GetTemperature());
	ASSERT_EQ(0, sim.GetWorld(0)->GetTemperature());
	ASSERT_THROW(sim.GetWorld(1), std::out_of_range);

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
}