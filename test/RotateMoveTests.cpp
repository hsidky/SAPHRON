#include "../src/Moves/RotateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Moves/MoveManager.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"
#include <math.h>

using namespace SAPHRON;

// Tests rotations on individual sites.
TEST(RotateMove, SiteRotations)
{
	Site s({0, 0, 0}, {0, 0, 1}, "T1");
	RotateMove m(3.1415);

	// x-axis rotations.
	m.Rotate(&s, 1, M_PI);
	ASSERT_TRUE(is_close(Director({0, 0, -1}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 1, -M_PI);
	ASSERT_TRUE(is_close(Director({0, 0, 1}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 1, -M_PI/2.0);
	ASSERT_TRUE(is_close(Director({0, -1, 0}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 1, M_PI/4.0);
	ASSERT_TRUE(is_close(Director({0, -sqrt(2)/2.0, sqrt(2)/2.0}), s.GetDirector(),1e-11)); 

	// y-axis rotations.	
	m.Rotate(&s, 2, M_PI/4);
	ASSERT_TRUE(is_close(Director({-0.5, -sqrt(2)/2.0, 0.5}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 2, -M_PI/2);
	ASSERT_TRUE(is_close(Director({0.5, -sqrt(2)/2.0, 0.5}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 2, -M_PI/4);
	ASSERT_TRUE(is_close(Director({sqrt(2)/2.0, -sqrt(2)/2.0, 0}), s.GetDirector(),1e-11)); 

	// z-axis rotations.
	m.Rotate(&s, 3, M_PI/4);
	ASSERT_TRUE(is_close(Director({0, -1, 0}), s.GetDirector(),1e-11)); 
	m.Rotate(&s, 3, -M_PI/2);
	ASSERT_TRUE(is_close(Director({1, 0, 0}), s.GetDirector(),1e-11)); 

}

// Test rotations on molecules
TEST(RotateMove, MoleculeRotations)
{
	Site* s1 = new Site({1, 0, 0}, {0, 0, 1}, "T1");	
	Site* s2 = new Site({-1, 0, 0}, {0, 0, 1}, "T1");
	Site* s3 = new Site({0, 0, 1}, {0, 0, 1}, "T1");
	Site* s4 = new Site({0, 0, -1}, {0, 0, 1}, "T1");
	Molecule m1("M1");
	m1.AddChild(s1);
	m1.AddChild(s2);
	m1.AddChild(s3);
	m1.AddChild(s4);

	RotateMove mv(3.1415);

	// COM should be at origin.
	ASSERT_TRUE(is_close(Position({0, 0, 0}), m1.GetPosition(),1e-11)); 

	// Rotate 90 degrees about y.
	mv.Rotate(&m1, 2, M_PI/2);
	m1.UpdateCenterOfMass();
	ASSERT_TRUE(is_close(Position({0, 0,0}), m1.GetPosition(), 1e-11));
	ASSERT_TRUE(is_close(Position({0, 0, 1}), s1->GetPosition(), 1e-11));
	ASSERT_TRUE(is_close(Position({0, 0, -1}), s2->GetPosition(), 1e-11));
	ASSERT_TRUE(is_close(Position({-1, 0, 0}), s3->GetPosition(), 1e-11));
	ASSERT_TRUE(is_close(Position({1, 0, 0}), s4->GetPosition(), 1e-11));
	ASSERT_TRUE(is_close(Director({-1, 0, 0}), s1->GetDirector(), 1e-11));
	ASSERT_TRUE(is_close(Director({-1, 0, 0}), s2->GetDirector(), 1e-11));
	ASSERT_TRUE(is_close(Director({-1, 0, 0}), s3->GetDirector(), 1e-11));
	ASSERT_TRUE(is_close(Director({-1, 0, 0}), s4->GetDirector(), 1e-11));
}

TEST(RotateMove, MoveInterface)
{
	World w(10.0, 10.0, 10.0, 2.0, 1.0);

	// Create a new site.
	Site* s1 = new Site({0, 0, 0}, {0, 0, 1}, "T1");	
	w.AddParticle(s1);

	// Max rotation of 45 degrees.
	RotateMove mv(M_PI/4.0);

	MoveManager mm;
	mm.AddMove(&mv);

	ForceFieldManager ffm;
	WorldManager wm;
	wm.AddWorld(&w);

	// Difference between directors should never exceed dmax.
	for (int i = 0; i < 100000; ++i)
	{
		Director d = s1->GetDirector();
		mv.Perform(&wm, &ffm, MoveOverride::ForceAccept);
		auto dot = fdot(d,s1->GetDirector());
		ASSERT_LE(acos(dot), M_PI/4.0);
	}

	for(int i = 0; i < 1000; ++i)
	{
		Director d = s1->GetDirector();
		mv.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_TRUE(is_close(d, s1->GetDirector(), 1e-11));
	}
}