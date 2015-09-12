#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"

#include <map>

using namespace SAPHRON;

TEST(SimpleWorld, DefaultBehavior)
{
	int n = 30;
	SimpleWorld world(n, n, n, 1.0);
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
	
	world.UpdateNeighborList();
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		const Position& coords = particle->GetPositionRef();

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

		auto& neighbors = particle->GetNeighbors();
		ASSERT_EQ(6, (int)neighbors.size());

		for(auto& neighbor : neighbors)
		{
			auto& np = neighbor->GetPositionRef();
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

	// Update list again to make sure lists are being properly cleared.
	world.UpdateNeighborList();
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		const Position& coords = particle->GetPositionRef();

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

		auto& neighbors = particle->GetNeighbors();
		ASSERT_EQ(6, (int)neighbors.size());

		for(auto& neighbor : neighbors)
		{
			auto& np = neighbor->GetPositionRef();
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
	
	// Check the composition of the world to make sure it's correct.
	auto& composition = world.GetComposition();

	ASSERT_EQ(9000, composition.at(site1.GetSpeciesID()));
	ASSERT_EQ(9000, composition.at(site2.GetSpeciesID()));
	ASSERT_EQ(9000, composition.at(site3.GetSpeciesID()));

	// Test changing species, adding particle, removing particle. 
	Particle* pr = world.DrawRandomParticle();
	world.RemoveParticle(pr);
	ASSERT_EQ(8999, composition.at(pr->GetSpeciesID()));

	world.AddParticle(pr);
	ASSERT_EQ(9000, composition.at(pr->GetSpeciesID()));

	int previd = pr->GetSpeciesID();
	int newid = (previd == 0) ? 1 : 0;
	pr->SetSpecies(newid);
	ASSERT_EQ(9001, composition.at(newid));
	ASSERT_EQ(8999, composition.at(previd));
}

TEST(SimpleWorld, MoveParticleSemantics)
{
	int n = 30;
	SimpleWorld world(n, n, n, 1);

	ASSERT_EQ(0, world.GetParticleCount());
	world.AddParticle(new Site({0,0,0}, {1,0,0}, "E1"));
	ASSERT_EQ(1, world.GetParticleCount());
	auto * p = world.SelectParticle(0);
	Director d {1, 0, 0};
	ASSERT_EQ(d, p->GetDirector());
}

TEST(SimpleWorld, VolumeScaling)
{
	int n = 30;
	SimpleWorld world(n, n, n, 1);
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	world.PackWorld({&site1}, {1.0}, 500, 1.0);
	ASSERT_EQ(500, world.GetParticleCount());

	// Get random coordinate and check it afterwards.
	auto box = world.GetBoxVectors();
	Particle* p = world.DrawRandomParticle();
	Position newpos = 2.0*p->GetPosition(); // we will scale by 2
	world.SetBoxVectors(2.0*box.x, 2.0*box.y, 2.0*box.z, true);
	ASSERT_EQ(newpos, p->GetPosition());
}

TEST(SimpleWorld, NeighborList)
{
	// create world with specified nlist. 
	double rcut = 1.5;
	SimpleWorld world(30, 30, 30, rcut);
	ASSERT_EQ(rcut, world.GetCutoffRadius());
	ASSERT_DOUBLE_EQ(1.3*rcut, world.GetNeighborRadius());
	ASSERT_DOUBLE_EQ(0.3*rcut, world.GetSkinThickness());

	// Change values and make sure it propogates. 
	world.SetCutoffRadius(0.9);
	ASSERT_DOUBLE_EQ(0.9, world.GetCutoffRadius());
	ASSERT_DOUBLE_EQ(1.3*rcut-0.9, world.GetSkinThickness());
}