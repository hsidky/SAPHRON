#include "../src/Site.h"
#include "../src/Moves/FlipSpinMove.h"
#include "gtest/gtest.h"

using namespace Moves;

// Test FlipSpinMove default behavior.
TEST(FlipSpinMove, DefaultBehavior)
{
    Site s(0, 0, 0);
    FlipSpinMove m;

    // Set the Z unit vector and test move
    s.SetZUnitVector(-1.0);
    m.Perform(s);
    ASSERT_EQ(1.0, s.GetZUnitVector());
    m.Undo();
    ASSERT_EQ(-1.0, s.GetZUnitVector());
}
