#include "../src/NewMoves/TranslateMove.h"
#include "../src/Particles/NewParticle.h"
#include "../src/Worlds/NewWorld.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/NewForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"
#include <cmath>
#include <numeric>

/*
namespace SAPHRON
{
	class TranslateParticleCounter : public ParticleObserver
	{
		public:
			std::map<std::string, int> movecounts;

			TranslateParticleCounter() : movecounts()
			{}

			virtual void ParticleUpdate(const ParticleEvent& pEvent) override
			{
				auto* p = pEvent.GetParticle();
				auto s = p->GetSpecies();
				
				if (pEvent.position)
				{
					if(movecounts.find(s) == movecounts.end())
						movecounts[s] = 1;
					else
						++movecounts[s];
				}
			}
	};
}
*/

using namespace SAPHRON;

// Test TranslateMove default behavior
TEST(TranslateMove, DefaultBehavior)
{
	std::vector<Site> sites;
	sites.push_back(Site());
	NewParticle particle(1, {0}, &sites);

	auto dx = 0.75;
	TranslateMove m(dx);

	// Add one particle to the world and add the world to the world manager.
	NewWorld world(10.0, 10.0, 10.0, 5.0, 1.0, 3243);
	world.BuildCellList();
	world.AddParticle(particle);
	
	WorldManager wm;
	wm.AddWorld(&world);

	// Empty forcefield manager.
	ForceFieldManager ffm;

	// Do a bunch of these for good measure.
	auto p = world.SelectParticle(0);
	for(int i = 0; i < 100000; ++i)
	{
		auto prev = p->GetPosition();
		m.Perform(&wm, &ffm, Accept);
		auto curr = p->GetPosition();
		Vector3 diff = curr - prev;
		world.ApplyMinimumImage(diff);
		ASSERT_LE(diff.norm(), dx);
	}

	ASSERT_EQ(1.0, m.GetAcceptanceRatio());
}

/*
TEST(TranslateMove, Selector)
{
	World world(10.0, 10.0, 10.0, 5.0, 0.0);
	WorldManager wm;
	wm.AddWorld(&world);

	Position p1 = {0, 0, 1.0};
	Position p2 = {0, 0, 0};

	Particle s1(p1, {0, 0, 0}, "S1");
	Particle s2(p2, {0, 0, 0}, "S2");

	for(size_t i = 0; i < 100; ++i)
	{
		world.AddParticle(s1.Clone());
		world.AddParticle(s2.Clone());
	}

	ASSERT_EQ(200, world.GetParticleCount());
	
	// Empty forcefield manager.
	ForceFieldManager ffm;

	// Try to translate s1 only. 
	std::map<int, double> dx = {{s1.GetSpeciesID(), 0.5}, {s2.GetSpeciesID(), 0.0}};

	// Not explicit first.
	TranslateMove m1(dx, false);

	// Perform the move lots.
	for(int i = 0; i < 10000; ++i)
		m1.Perform(&wm, &ffm, MoveOverride::ForceAccept);

	// Make sure that s1 moved, but s2 stayed where it is.
	for(auto& p : world)
	{
		if(p->GetSpecies() == "S1")
			ASSERT_FALSE(is_close(p1, p->GetPosition(), 1e-10));
		else
			ASSERT_TRUE(is_close(p2, p->GetPosition(), 1e-10));
	}
}

TEST(TranslateMove, ExplicitDraw)
{
	World world(10.0, 10.0, 10.0, 5.0, 0.0);
	WorldManager wm;
	wm.AddWorld(&world);

	Position p1 = {0, 0, 1.0};
	Position p2 = {0, 0, 0};

	Particle s1(p1, {0, 0, 0}, "S1");
	Particle s2(p2, {0, 0, 0}, "S2");
	Particle s3(p2, {0, 0, 0}, "S3");

	world.PackWorld({&s1, &s2, &s3}, {0.1, 0.3, 0.6}, 1000, 0.01);

	ASSERT_EQ(1000, world.GetParticleCount());

	auto& comp = world.GetComposition();
	ASSERT_EQ(100, comp[s1.GetSpeciesID()]);
	ASSERT_EQ(300, comp[s2.GetSpeciesID()]);
	ASSERT_EQ(600, comp[s3.GetSpeciesID()]);
	

	// Add particle counter to all particles.
	TranslateParticleCounter counter;
	for(auto& p : world)
		p->AddObserver(&counter);

	// Empty forcefield manager.
	ForceFieldManager ffm;
	std::map<int, double> dx = {{s1.GetSpeciesID(), 0.5}, {s2.GetSpeciesID(), 0.7}, {s3.GetSpeciesID(), 0.3}};

	// Explicit draw.
	TranslateMove m1(dx, true);

	// Perform the move lots.
	for(int i = 0; i < 10000; ++i)
		m1.Perform(&wm, &ffm, MoveOverride::ForceAccept);

	// Ratio of moves performed to composition should be close.
	ASSERT_NEAR(counter.movecounts["S1"]/10000., 0.1, 1e-2);
	ASSERT_NEAR(counter.movecounts["S2"]/10000., 0.3, 1e-2);
	ASSERT_NEAR(counter.movecounts["S3"]/10000., 0.6, 1e-2);
}
*/