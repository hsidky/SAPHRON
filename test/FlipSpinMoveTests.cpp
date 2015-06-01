#include "../src/Moves/FlipSpinMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

// Test FlipSpinMove default behavior.
TEST(FlipSpinMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	FlipSpinMove m;

	// Set the Z unit vector and test move
	s.SetDirector({-1.0,0,0});
	m.Perform({&s});
	ASSERT_EQ(1.0, s.GetDirector().x);
	m.Undo();
	ASSERT_EQ(-1.0, s.GetDirector().x);
}

// Test reusability of move on different sites.
TEST(FlipSpinMove, ChangeSiteReusability)
{
	Site s1({0, 0, 0},{0,0,0}, "T1");
	Site s2({0, 0, 0},{0,0,0}, "T1");
	FlipSpinMove m;

	// Set the Z unit vector and test move
	s1.SetDirector({-1.0,0,0});
	m.Perform({&s1});
	ASSERT_EQ(1.0, s1.GetDirector().x);
	m.Undo();
	ASSERT_EQ(-1.0, s1.GetDirector().x);

	s2.SetDirector({-3.0,0,0});
	m.Perform({&s2});
	ASSERT_EQ(3.0, s2.GetDirector().x);
	m.Undo();
	ASSERT_EQ(-3.0, s2.GetDirector().x);
}

TEST(FlipSpinMove, PassPointerByReference)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	FlipSpinMove m;

	Site* sp = &s;

	// Set the Z unit vector and test move
	sp->SetDirector({-1.0,0,0});
	m.Perform({sp});
	ASSERT_EQ(1.0, sp->GetDirector().x);
	m.Undo();
	ASSERT_EQ(-1.0, sp->GetDirector().x);
}
