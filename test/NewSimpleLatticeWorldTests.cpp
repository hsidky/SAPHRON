#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleLatticeWorld.h"
#include "gtest/gtest.h"

#include <map>

using namespace SAPHRON;

TEST(SimpleLatticeWorld, DefaultConstructor)
{
	SimpleLatticeWorld world(30, 30, 30, 1);
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	Site site2({0, 0, 0}, {0, 1, 0}, "E2");
	Site site3({0, 0, 0}, {0, 0, 1}, "E3");

	world.ConfigureParticles({&site1, &site2, &site3}, {1.0/3.0, 1.0/3.0, 1.0/3.0});

	ASSERT_EQ(27000, world.GetParticleCount());

	std::map<std::string, int> counts {{"E1", 0}, {"E2", 0}, {"E3", 0}};
	for(int i = 0; i < world.GetParticleCount(); i++)
	{
		auto particle = world.GetParticle(i);
		counts[particle->GetIdentifier()]++;
	}

	ASSERT_EQ(9000, counts["E1"]);
	ASSERT_EQ(9000, counts["E2"]);
	ASSERT_EQ(9000, counts["E3"]);

	world.ConfigureNeighborList();
	for(int i = 0; i < world.GetParticleCount(); i++)
	{
		auto particle = world.GetParticle(i);
		ASSERT_EQ(6, particle->GetNeighborList().size());
	}
}
