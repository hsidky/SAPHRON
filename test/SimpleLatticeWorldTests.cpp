#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleLatticeWorld.h"
#include "gtest/gtest.h"

#include <map>

using namespace SAPHRON;

TEST(SimpleLatticeWorld, DefaultConstructor)
{
	int n = 30;
	SimpleLatticeWorld world(n, n, n, 1);
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	Site site2({0, 0, 0}, {0, 1, 0}, "E2");
	Site site3({0, 0, 0}, {0, 0, 1}, "E3");

	world.ConfigureParticles({&site1, &site2, &site3}, {1.0/3.0, 1.0/3.0, 1.0/3.0});

	ASSERT_EQ(27000, world.GetParticleCount());

	std::map<std::string, int> counts {{"E1", 0}, {"E2", 0}, {"E3", 0}};
	for(int i = 0; i < world.GetParticleCount(); i++)
	{
		auto particle = world.SelectParticle(i);
		counts[particle->GetSpecies()]++;
	}

	ASSERT_EQ(9000, counts["E1"]);
	ASSERT_EQ(9000, counts["E2"]);
	ASSERT_EQ(9000, counts["E3"]);

	world.ConfigureNeighborList();
	for(int i = 0; i < world.GetParticleCount(); i++)
	{
		auto particle = world.SelectParticle(i);
		Position coords = particle->GetPosition();

		// Check neighbors
		Position n1 =
		{(coords.x == n) ? 1.0 : (double) coords.x + 1.0, (double) coords.y, (double) coords.z};
		Position n2 =
		{(coords.x == 1) ? (double) n : coords.x - 1.0, (double) coords.y, (double) coords.z};
		Position n3 =
		{(double) coords.x, (coords.y == n) ? 1.0 : (double) coords.y + 1.0, (double) coords.z};
		Position n4 =
		{(double) coords.x, (coords.y == 1) ? (double) n : coords.y - 1.0, (double) coords.z};
		Position n5 =
		{(double) coords.x, (double) coords.y, (coords.z == n) ? 1.0 : (double) coords.z + 1.0};
		Position n6 =
		{(double) coords.x, (double) coords.y, (coords.z == 1) ? (double) n : coords.z - 1.0};

		auto neighbors = particle->GetNeighbors();
		ASSERT_EQ(6, neighbors.size());

		for(auto& neighbor : neighbors)
		{
			auto np = neighbor.GetParticle()->GetPosition();
			ASSERT_TRUE(
			        np == n1 ||
			        np == n2 ||
			        np == n3 ||
			        np == n4 ||
			        np == n5 ||
			        np == n6
			        );
		}
	}
}
