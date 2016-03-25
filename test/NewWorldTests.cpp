#include "../src/Worlds/NewWorld.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(World, AddRemoveParticle)
{
	// Create particle with 2 sites.
	std::vector<Site> sites;
	sites.push_back(Site());
	sites.push_back(Site());
	sites[0].species = 0;
	sites[1].species = 1;

	NewParticle p1(2, {0, 1}, &sites);

	// Create world and add twice.
	NewWorld world(10., 10., 10., 5., 0);
	ASSERT_EQ(0, world.GetParticleCount());
	ASSERT_EQ(0, world.GetSiteCount());
	world.AddParticle(p1);
	world.AddParticle(p1);
	ASSERT_EQ(2, world.GetParticleCount());
	ASSERT_EQ(4, world.GetSiteCount());

	// Check composition. 
	auto& pcomps = world.GetParticleCompositions();
	auto& scomps = world.GetSiteCompositions();
	ASSERT_EQ(2, pcomps[p1.GetSpecies()]);
	ASSERT_EQ(2, scomps[p1.GetSpecies(0)]);
	ASSERT_EQ(2, scomps[p1.GetSpecies(1)]);

	// Remove particle.
	world.RemoveParticle(*world.SelectParticle(0));
	ASSERT_EQ(1, world.GetParticleCount());
	ASSERT_EQ(2, world.GetSiteCount());
	ASSERT_EQ(1, pcomps[p1.GetSpecies()]);
	ASSERT_EQ(1, scomps[p1.GetSpecies(0)]);
	ASSERT_EQ(1, scomps[p1.GetSpecies(1)]);
}

TEST(World, PackBox)
{
	NewWorld world(1.0, 1.0, 1.0, 1.0, 1.0);

	// Create three particles each with 1 site.
	std::vector<Site> sites;
	sites.push_back(Site());
	sites.push_back(Site());
	sites.push_back(Site());

	sites[0].species = 0;
	sites[1].species = 1;
	sites[2].species = 2; 

	NewParticle p1(3, {0}, &sites);
	NewParticle p2(4, {1}, &sites);
	NewParticle p3(5, {2}, &sites);

	world.PackWorld({&p1, &p2, &p3}, {1./3., 1./3., 1./3.}, 4500, 0.5);
	ASSERT_EQ(4500, world.GetParticleCount());
	ASSERT_EQ(4500, world.GetSiteCount());
}