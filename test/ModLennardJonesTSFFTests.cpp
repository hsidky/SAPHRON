#include "../src/ForceFields/ModLennardJonesTSFF.h"
#include "../src/Particles/Particle.h"

#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace SAPHRON;

TEST(ModLennardJonesTSFF, DefaultBehavior)
{
	double rcut = pow(2.0, 1.0/6.0);
	ModLennardJonesTSFF ff1(1.0, 1.0, 3.0, {rcut});
	ModLennardJonesTSFF ff2(1.0, 1.0, 0.0, {rcut});
	Particle s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Particle s2({rcut, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	s1.AddNeighbor(&s2);
	s2.AddNeighbor(&s1);

	auto V = ff1.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_NEAR(0.0, V.energy, 1E-10);
	ASSERT_NEAR(0.0, V.virial,1E-10);

	// Check position less than rc. 
	s2.SetPosition({0.9*rcut, 0.0, 0.0});
	V = ff1.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_LT(V.energy, 0);
	ASSERT_GT(V.virial, 0);

	// Check Beta
	auto V2 = ff2.Evaluate(s1,s2,s1.GetPosition()-s2.GetPosition(),0);
	ASSERT_GT(V2.energy,V.energy);
	ASSERT_LT(V2.virial,V.virial);
}
