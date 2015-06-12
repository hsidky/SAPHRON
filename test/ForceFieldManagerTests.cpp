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

	int i = 0;
	for(auto forcefield : ffm){
		if(forcefield.second != nullptr)
			++i;
	}

	ASSERT_EQ(2, i);
}