#include "../src/ForceFields/LennardJonesTSFF.h"
#include "../src/Particles/Particle.h"

#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace SAPHRON;

TEST(LennardJonesFF, DefaultBehavior)
{
	double rcut = pow(2.0, 1.0/6.0);
	LennardJonesTSFF ff(1.0, 1.0, {rcut});
	Particle s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Particle s2({rcut, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	s1.AddNeighbor(&s2);
	s2.AddNeighbor(&s1);

	auto V = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_DOUBLE_EQ(0.0, V.energy);
	ASSERT_DOUBLE_EQ(0.0, V.virial);

	// Check position less than rc. 
	s2.SetPosition({0.5*rcut, 0.0, 0.0});
	V = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_GT(V.energy, 0);
	ASSERT_LT(V.virial, 0);
}
