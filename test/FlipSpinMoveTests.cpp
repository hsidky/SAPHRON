#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Moves;

// Test FlipSpinMove default behavior.
TEST(FlipSpinMove, DefaultBehavior)
{
	Site s(0, 0, 0, 0);
	FlipSpinMove m;

	// Set the Z unit vector and test move
	s.SetZUnitVector(-1.0);
	m.Perform(s);
	ASSERT_EQ(1.0, s.GetZUnitVector());
	m.Undo();
	ASSERT_EQ(-1.0, s.GetZUnitVector());
}

// Test reusability of move on different sites.
TEST(FlipSpinMove, ChangeSiteReusability)
{
	Site s1(0, 0, 0, 0);
	Site s2(0, 0, 0, 0);
	FlipSpinMove m;

	// Set the Z unit vector and test move
	s1.SetZUnitVector(-1.0);
	m.Perform(s1);
	ASSERT_EQ(1.0, s1.GetZUnitVector());
	m.Undo();
	ASSERT_EQ(-1.0, s1.GetZUnitVector());

	s2.SetZUnitVector(-3.0);
	m.Perform(s2);
	ASSERT_EQ(3.0, s2.GetZUnitVector());
	m.Undo();
	ASSERT_EQ(-3.0, s2.GetZUnitVector());
}

TEST(FlipSpinMove, PassPointerByReference)
{
	Site s(0, 0, 0, 0);
	FlipSpinMove m;

	Site* sp = &s;

	// Set the Z unit vector and test move
	sp->SetZUnitVector(-1.0);
	m.Perform(*sp);
	ASSERT_EQ(1.0, sp->GetZUnitVector());
	m.Undo();
	ASSERT_EQ(-1.0, sp->GetZUnitVector());
}
