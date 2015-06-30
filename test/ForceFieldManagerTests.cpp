#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/ForceFields/FENEFF.h"
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
	FENEFF Fff(1.0, 2.0, 3.0, 4.0);
	FENEFF Fff2(4.0, 3.0, 2.0, 1.0);


	// Define ff manager.
	ForceFieldManager ffm;

	ffm.AddNonBondedForceField("L1", "L1", ff);
	ffm.AddNonBondedForceField("L1", "L2", ff2);
	ffm.AddBondedForceField("L1", "L1", Fff);
	ffm.AddBondedForceField("L1", "L2", Fff2);

	// Note: cannot set volume to zero (or else D.B.Z).
	ASSERT_EQ(-4.0, ffm.EvaluateHamiltonian(s1, {}, 10).energy.total());
	ASSERT_EQ(0.0, ffm.EvaluateHamiltonian(s1, {}, 10).pressure.isotropic());

	// TODO: Need to add a test for FENEFF?
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
	FENEFF Fff(1.0, 2.0, 3.0, 4.0);
	FENEFF Fff2(4.0, 3.0, 2.0, 1.0);


	// Define ff manager.
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("J1", "J1", ff);
	ffm.AddNonBondedForceField("J1", "J2", ff2);
	ffm.AddBondedForceField("J1", "J1", Fff);
	ffm.AddBondedForceField("J1", "J2", Fff2);

	ASSERT_EQ(2, ffm.NonBondedForceFieldCount());
	ASSERT_EQ(2, ffm.BondedForceFieldCount());

	int i = 0;
	for(auto& forcefield : ffm.GetNonBondedForceFields()){
		if(forcefield.second != nullptr)
			++i;
	}

	ASSERT_EQ(2, i);

	// Remove forcefield. Check count. 
	ffm.RemoveNonBondedForceField("J1", "J1");
	ASSERT_EQ(1, ffm.NonBondedForceFieldCount());
	ffm.RemoveBondedForceField("J1", "J1");
	ASSERT_EQ(1, ffm.BondedForceFieldCount());

}