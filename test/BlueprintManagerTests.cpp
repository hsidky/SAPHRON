#include "../src/Particles/BlueprintManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(BlueprintManager, DefaultBehavior)
{
	// Get instance of blueprint manager. 
	auto& bp = BlueprintManager::Instance();

	// Manager is empty. 
	ASSERT_EQ(0, bp.GetBlueprintCount());
	ASSERT_EQ(0, bp.GetBlueprintSize());
	ASSERT_EQ(0, bp.GetNextID());

	// Create site and add to blueprint manager species index. 
	Site site1; 
	site1.species = bp.GetNextID();
	bp.AddSpeciesIndex("S1", site1.species);
	ASSERT_EQ(0, site1.species);
	ASSERT_EQ(1, bp.GetNextID());
	ASSERT_TRUE(bp.IsRegistered("S1"));
	ASSERT_EQ(0, bp.GetSpeciesIndex("S1"));

	// Add another site to a non-consecutive index. 
	Site site2; 
	site2.species = 2;
	bp.AddSpeciesIndex("S2", site2.species);
	ASSERT_EQ(2, site2.species);
	ASSERT_TRUE(bp.IsRegistered("S2"));
	ASSERT_EQ(2, bp.GetSpeciesIndex("S2"));
	ASSERT_EQ(1, bp.GetNextID()); // 1 should still be free.

	// Fill in the empty spot with site 3.
	Site site3;
	site3.species = bp.GetNextID();
	bp.AddSpeciesIndex("S3", site3.species);
	ASSERT_EQ("S3", bp.GetSpeciesString(site3.species));
	ASSERT_EQ(1, site3.species);

	// There should still be no registered blueprints.
	ASSERT_EQ(0, bp.GetBlueprintCount());
	ASSERT_EQ(0, bp.GetBlueprintSize());

	// Create particle and add blueprint.
	site1.position = {1, 1, -2};
	site2.position = {1, 1, 2};
	std::vector<Site> psites{site1, site2};
	NewParticle p1{bp.GetNextID(), {0, 1}, &psites};
	bp.AddBlueprint("P1", p1);
	ASSERT_EQ(1, bp.GetBlueprintCount());
	ASSERT_EQ(4, bp.GetBlueprintSize());
	ASSERT_EQ(3, p1.GetSpecies());
	ASSERT_TRUE(bp.IsRegistered("P1"));

	// Get copy from blueprint.
	std::vector<Site> sites;
	NewParticle p1c = bp.GetBlueprint("P1", sites);
	ASSERT_EQ(3, p1c.GetSpecies());
	// Blueprint should be centered in local frame.
	Vector3 pos{0, 0, 0};
	ASSERT_EQ(pos, p1c.GetPosition());
	pos = {0, 0, -2};
	ASSERT_EQ(pos, p1c.GetPosition(0));
	pos = {0, 0, 2};
	ASSERT_EQ(pos, p1c.GetPosition(1));

	// Remove species from bp.
	bp.RemoveSpeciesIndex("P1");
	ASSERT_FALSE(bp.IsRegistered("P1"));
}