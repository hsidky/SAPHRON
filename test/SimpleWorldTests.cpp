#include "../src/Particles/Site.h"
#include "../src/Worlds/World.h"
#include "gtest/gtest.h"
#include <map>
#include <chrono>
#include <ctime>

using namespace SAPHRON;


TEST(SimpleWorld, NeighborList)
{
	// create world with specified nlist. 
	double rcut = 1.5;
	World world(30, 30, 30, rcut);
	ASSERT_EQ(rcut, world.GetCutoffRadius());
	ASSERT_DOUBLE_EQ(1.3*rcut, world.GetNeighborRadius());
	ASSERT_DOUBLE_EQ(0.3*rcut, world.GetSkinThickness());

	// Change values and make sure it propogates. 
	world.SetCutoffRadius(0.9);
	ASSERT_DOUBLE_EQ(0.9, world.GetCutoffRadius());
	ASSERT_DOUBLE_EQ(1.3*rcut-0.9, world.GetSkinThickness());

	// Pack a world with a decent number of of particles.
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	world.PackWorld({&site1}, {1.0}, 5000, 0.5);


    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    for(int i = 0; i < 100; ++i)
    	world.UpdateNeighborList();

    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
}

TEST(SimpleWorld, DefaultBehavior)
{
	int n = 30;
	World world(n, n, n, 1.0);
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	Site site2({0, 0, 0}, {0, 1, 0}, "E2");
	Site site3({0, 0, 0}, {0, 0, 1}, "E3");

	world.PackWorld({&site1, &site2, &site3}, {1.0/3.0, 1.0/3.0, 1.0/3.0});

	ASSERT_EQ(27000, world.GetParticleCount());
	ASSERT_EQ(27000, world.GetPrimitiveCount());


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
		const Position& coords = particle->GetPosition();

		// Check neighbors
		Position n1 =
		{(coords[0] == n) ? 1.0 : (double) coords[0] + 1.0, (double) coords[1], (double) coords[2]};
		Position n2 =
		{(coords[0] == 1) ? (double) n : coords[0] - 1.0, (double) coords[1], (double) coords[2]};
		Position n3 =
		{(double) coords[0], (coords[1] == n) ? 1.0 : (double) coords[1] + 1.0, (double) coords[2]};
		Position n4 =
		{(double) coords[0], (coords[1] == 1) ? (double) n : coords[1] - 1.0, (double) coords[2]};
		Position n5 =
		{(double) coords[0], (double) coords[1], (coords[2] == n) ? 1.0 : (double) coords[2] + 1.0};
		Position n6 =
		{(double) coords[0], (double) coords[1], (coords[2] == 1) ? (double) n : coords[2] - 1.0};

		auto& neighbors = particle->GetNeighbors();
		ASSERT_EQ(6, (int)neighbors.size());

		for(auto& neighbor : neighbors)
		{
			auto& np = neighbor->GetPosition();
			ASSERT_TRUE(
			        is_close(np, n1, 1e-11) ||
			        is_close(np, n2, 1e-11) ||
			        is_close(np, n3, 1e-11) ||
			        is_close(np, n4, 1e-11) ||
			        is_close(np, n5, 1e-11) ||
			        is_close(np, n6, 1e-11)
			        );
		}
	}

	// Update list again to make sure lists are being properly cleared.
	world.UpdateNeighborList();
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		const Position& coords = particle->GetPosition();

		// Check neighbors
		Position n1 =
		{(coords[0] == n) ? 1.0 : (double) coords[0] + 1.0, (double) coords[1], (double) coords[2]};
		Position n2 =
		{(coords[0] == 1) ? (double) n : coords[0] - 1.0, (double) coords[1], (double) coords[2]};
		Position n3 =
		{(double) coords[0], (coords[1] == n) ? 1.0 : (double) coords[1] + 1.0, (double) coords[2]};
		Position n4 =
		{(double) coords[0], (coords[1] == 1) ? (double) n : coords[1] - 1.0, (double) coords[2]};
		Position n5 =
		{(double) coords[0], (double) coords[1], (coords[2] == n) ? 1.0 : (double) coords[2] + 1.0};
		Position n6 =
		{(double) coords[0], (double) coords[1], (coords[2] == 1) ? (double) n : coords[2] - 1.0};

		auto& neighbors = particle->GetNeighbors();
		ASSERT_EQ(6, (int)neighbors.size());

		for(auto& neighbor : neighbors)
		{
			auto& np = neighbor->GetPosition();
			ASSERT_TRUE(
			        is_close(np, n1, 1e-11) ||
			        is_close(np, n2, 1e-11) ||
			        is_close(np, n3, 1e-11) ||
			        is_close(np, n4, 1e-11) ||
			        is_close(np, n5, 1e-11) ||
			        is_close(np, n6, 1e-11)
			        );
		}
	}
	
	// Check the composition of the world to make sure it's correct.
	auto& composition = world.GetComposition();

	ASSERT_EQ(9000, composition[site1.GetSpeciesID()]);
	ASSERT_EQ(9000, composition[site2.GetSpeciesID()]);
	ASSERT_EQ(9000, composition[site3.GetSpeciesID()]);

	// Test changing species, adding particle, removing particle. 
	Particle* pr = world.DrawRandomParticle();
	world.RemoveParticle(pr);
	ASSERT_EQ(8999, composition[pr->GetSpeciesID()]);

	world.AddParticle(pr);
	ASSERT_EQ(9000, composition[pr->GetSpeciesID()]);

	int previd = pr->GetSpeciesID();
	int newid = (previd == 0) ? 1 : 0;
	pr->SetSpeciesID(newid);
	ASSERT_EQ(9001, composition[newid]);
	ASSERT_EQ(8999, composition[previd]);
}

TEST(SimpleWorld, MoveParticleSemantics)
{
	int n = 30;
	World world(n, n, n, 1);

	ASSERT_EQ(0, world.GetParticleCount());
	world.AddParticle(new Site({0,0,0}, {1,0,0}, "E1"));
	ASSERT_EQ(1, world.GetParticleCount());
	ASSERT_EQ(1, world.GetPrimitiveCount());
	auto * p = world.SelectParticle(0);
	Director d {1, 0, 0};
	ASSERT_TRUE(is_close(d, p->GetDirector(), 1e-9));
}

TEST(SimpleWorld, VolumeScaling)
{
	int n = 30;
	World world(n, n, n, 1);
	Site site1({0, 0, 0}, {1, 0, 0}, "E1");
	world.PackWorld({&site1}, {1.0}, 500, 1.0);
	ASSERT_EQ(500, world.GetParticleCount());
	ASSERT_EQ(500, world.GetPrimitiveCount());

	// Get random coordinate and check it afterwards.
	auto box = world.GetHMatrix();
	Particle* p = world.DrawRandomParticle();
	Position newpos = 2.0*p->GetPosition(); // we will scale by 2
	world.SetVolume(8*world.GetVolume(), true);
	ASSERT_TRUE(is_close(newpos, p->GetPosition(),1e-11));
}