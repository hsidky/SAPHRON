#include "../src/Particles/Particle.h"

#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Molecule, DefaultBehavior)
{
	Particle* s1 = new Particle({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Particle* s2 = new Particle({2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L2");
	Particle* s3 = new Particle({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L3");

	Particle m("M1");

	m.AddChild(s1);
	m.AddChild(s2);
	m.AddChild(s3);

	Position pos{2.0, 0.0, 0.0};
	ASSERT_TRUE(is_close(pos, m.GetPosition(),1e-11));

	// Move molecule.
	pos = {3.5, 2.7, 8.3};
	m.SetPosition(pos);
	ASSERT_TRUE(is_close(pos, m.GetPosition(),1e-11));
	ASSERT_TRUE(is_close(Position({2.5, 2.7, 8.3}), s1->GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(Position({3.5, 2.7, 8.3}), s2->GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(Position({4.5, 2.7, 8.3}), s3->GetPosition(),1e-11)); 

	ASSERT_EQ(3.0, m.GetMass());


	// Remove children.
	m.RemoveChild(s1);
	pos = {4.0, 2.7, 8.3};
	ASSERT_TRUE(is_close(pos, m.GetPosition(),1e-10));

	delete s1;
}

TEST(Molecule, CopyConstructor)
{
	Particle* s1 = new Particle({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Particle* s2 = new Particle({2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Particle* s3 = new Particle({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	Particle m("M1");
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