#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Particles/Particle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(LebwohlLasherFF, DefaultConstructor)
{
	LebwohlLasherFF ff(1.0, 0);
	Particle s1({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L1");
	Particle s2({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, "L1");

	ASSERT_EQ(-1.0, ff.Evaluate(s1, s2, {0, 0, 0},0).energy);
	s2.SetDirector({0.0, 0.0, 1.0});
	ASSERT_EQ(0.5, ff.Evaluate(s1, s2, {0, 0, 0},0).energy);
}
