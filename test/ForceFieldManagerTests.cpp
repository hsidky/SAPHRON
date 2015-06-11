#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ForceFieldManager, DefaultBehavior)
{
	// Define sites.
	Site s1({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L1");
	Site s2({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L1");
	Site s3({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L1");
	Site s4({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L2");

	// Add neighbors.
	s1.AddNeighbor(&s2);
	s1.AddNeighbor(&s3);
	s1.AddNeighbor(&s4);

	// Define force field.
	LebwohlLasherFF ff(1.0, 0);
	LebwohlLasherFF ff2(2.0, 0);

	// Define ff manager.
	ForceFieldManager ffm;

	ffm.AddForceField("L1", "L1", ff);
	ffm.AddForceField("L1", "L2", ff2);

	// Note: cannot set volume to zero (or else D.B.Z).
	ASSERT_EQ(-4.0, ffm.EvaluateHamiltonian(s1, {}, 10).energy.total());
	ASSERT_EQ(0.0, ffm.EvaluateHamiltonian(s1, {}, 10).pressure.isotropic());
}

TEST(ForceFieldManager, FFIterator)
{
	Site s1({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "J1");
	Site s2({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "J1");
	Site s3({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "J1");
	Site s4({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "J2");

	// Define force field.
	LebwohlLasherFF ff(1.0, 0);
	LebwohlLasherFF ff2(2.0, 0);

	// Define ff manager.
	ForceFieldManager ffm;
	ffm.AddForceField("J1", "J1", ff);
	ffm.AddForceField("J1", "J2", ff2);

	ASSERT_EQ(2, ffm.ForceFieldCount());

	// Pull both forcefields and verify.
	ASSERT_EQ(&ff, ffm.GetForceField(0));
	ASSERT_EQ(&ff2, ffm.GetForceField(1));
	ASSERT_EQ(nullptr, ffm.GetForceField(2));

	// Check registered species types.
	std::pair<int, int> type = {s1.GetSpeciesID(), s1.GetSpeciesID()};
	ASSERT_EQ(type, ffm.GetForceFieldTypes(0));
	type.first = 0;
	type.second = 1;
	ASSERT_EQ(type, ffm.GetForceFieldTypes(1));

	// Verify string types
	ASSERT_EQ("J1", Particle::GetSpeciesList()[type.first]);
	ASSERT_EQ("J2", Particle::GetSpeciesList()[type.second]);

	// Check invalid case.
	type.first = -1;
	type.second = -1;
	ASSERT_EQ(type, ffm.GetForceFieldTypes(2));
}