#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Molecule, DefaultBehavior)
{
	Site* s1 = new Site({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site* s2 = new Site({2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site* s3 = new Site({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	Molecule m("M1");

	m.AddChild(s1);
	m.AddChild(s2);
	m.AddChild(s3);

	Position pos{2.0, 0.0, 0.0};
	ASSERT_EQ(pos, m.GetPosition());

	// Move molecule.
	pos = {3.5, 2.7, 8.3};
	m.SetPosition(pos);
	ASSERT_EQ(pos, m.GetPosition());
	ASSERT_EQ(Position({2.5, 2.7, 8.3}), s1->GetPosition()); 
	ASSERT_EQ(Position({3.5, 2.7, 8.3}), s2->GetPosition()); 
	ASSERT_EQ(Position({4.5, 2.7, 8.3}), s3->GetPosition()); 

	ASSERT_EQ(3.0, m.GetMass());


	// Remove children.
	m.RemoveChild(s1);
	pos = {4.0, 2.7, 8.3};
	ASSERT_EQ(pos, m.GetPosition());

	delete s1;
}

TEST(Molecule, CopyConstructor)
{
	Site* s1 = new Site({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site* s2 = new Site({2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site* s3 = new Site({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	Molecule m("M1");
	m.AddChild(s1);
	m.AddChild(s2);
	m.AddChild(s3);

	s1->AddBondedNeighbor(s2);
	s2->AddBondedNeighbor(s1);
	s2->AddBondedNeighbor(s3);
	s3->AddBondedNeighbor(s2);

	// make a copy. 
	auto m2(m);

	// Check children.
	auto& children = m2.GetChildren();

	ASSERT_EQ(3, children.size());

	int i = 0;
	for(auto& child : children)
	{
		// Verify parent.
		ASSERT_EQ(&m2, child->GetParent());
		auto& bonded = child->GetBondedNeighbors();

		if(i == 0)
		{
			ASSERT_EQ(1, bonded.size());
			ASSERT_EQ(children[1], bonded[0]);
		}
		else if (i == 1)
		{
			ASSERT_EQ(2, bonded.size());
			ASSERT_EQ(children[0], bonded[0]);
			ASSERT_EQ(children[2], bonded[1]);
		}
		else if (i == 2)
		{
			ASSERT_EQ(1, bonded.size());
			ASSERT_EQ(children[1], bonded[0]);
		}
		++i;
	}
}