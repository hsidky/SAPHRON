#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Molecule, DefaultBehavior)
{
	Site s1({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s3({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	Molecule m("M1");

	m.AddChild(&s1);
	m.AddChild(&s2);
	m.AddChild(&s3);

	Position pos{2.0, 0.0, 0.0};
	ASSERT_EQ(pos, m.GetPosition());

	// Move molecule.
	pos = {3.5, 2.7, 8.3};
	m.SetPosition(pos);
	ASSERT_EQ(pos, m.GetPosition());
	ASSERT_EQ(Position({2.5, 2.7, 8.3}), s1.GetPosition()); 
	ASSERT_EQ(Position({3.5, 2.7, 8.3}), s2.GetPosition()); 
	ASSERT_EQ(Position({4.5, 2.7, 8.3}), s3.GetPosition()); 

	ASSERT_EQ(3.0, m.GetMass());


	// Remove children.
	m.RemoveChild(&s1);
	pos = {4.0, 2.7, 8.3};
	ASSERT_EQ(pos, m.GetPosition());
}