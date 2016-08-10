#include "gtest/gtest.h"
#include "Utils/Helpers.h"
#include "ForceFields/KernFrenkelFF.h"

using namespace SAPHRON;

TEST(KernFrenkelFF, DefaultBehavior)
{
	auto eps = 2.0, sigma = 1.0, delta = 1.0;
	std::vector<double> thetas{M_PI/3., M_PI/3.};
	std::vector<Vector3D> pjs{{0,0,1}, {0,0,-1}};
	KernFrenkelFF ff(eps, sigma, delta, thetas, pjs);

	Particle p1("T"), p2("T");
	p1.SetDirector({0, 0, 1});
	p2.SetDirector({0, 0, -1});
	p1.SetPosition({0, 0, 0});
	p2.SetPosition({0, 0, 0});
	
	// Hard sphere repulsion.
	p2.SetPosition({0, 0, 0.99*sigma});
	auto u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_GT(u.energy, 1e5);

	// Between sigma and delta.
	p2.SetPosition({0, 0, 0.5*(sigma+delta)});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(-eps, u.energy);

	// Greater than sigma + delta.
	p2.SetPosition({0, 0, 1.01*(sigma+delta)});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);

	// Set position and rotate.
	p2.SetPosition({0, 0, 1.01*sigma});

	// Patches are on both ends. Should be visible.
	p2.SetDirector({0, 0, 1});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	
	Vector3D nz = {0, 0, 1};
	auto dlim1 = thetas[0];
	
	// Just less than first patch size. 
	p2.SetDirector(GenRotationMatrix(1, 0.99*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(-eps, u.energy);

	// Just outside of patch.
	p2.SetDirector(GenRotationMatrix(1, 1.01*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);	

	// Just outside of second patch
	p2.SetDirector(GenRotationMatrix(1, M_PI-1.01*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);

	// Just inside of second patch
	p2.SetDirector(GenRotationMatrix(1, M_PI-0.99*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(-eps, u.energy);
}


TEST(KernFrenkelFF, InvertedPatch)
{
	auto eps = 2.0, sigma = 1.0, delta = 1.0;
	std::vector<double> thetas{M_PI/3., M_PI/3.};
	std::vector<Vector3D> pjs{{0,0,1}, {0,0,-1}};
	KernFrenkelFF ff(eps, sigma, delta, thetas, pjs, true);

	Particle p1("T"), p2("T");
	p1.SetDirector({0, 0, 1});
	p2.SetDirector({0, 0, -1});
	p1.SetPosition({0, 0, 0});
	p2.SetPosition({0, 0, 0});
	
	// Hard sphere repulsion.
	p2.SetPosition({0, 0, 0.99*sigma});
	auto u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_GT(u.energy, 1e5);

	// Between sigma and delta (compliment).
	p2.SetPosition({0, 0, 0.5*(sigma+delta)});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);

	// Greater than sigma + delta.
	p2.SetPosition({0, 0, 1.01*(sigma+delta)});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);

	// Set position and rotate.
	p2.SetPosition({0, 0, 1.01*sigma});

	// Patches are on both ends. Should be visible.
	p2.SetDirector({0, 0, 1});
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	
	Vector3D nz = {0, 0, 1};
	auto dlim1 = thetas[0];
	
	// Just less than first patch size. 
	p2.SetDirector(GenRotationMatrix(1, 0.99*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);

	// Just outside of patch.
	p2.SetDirector(GenRotationMatrix(1, 1.01*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(-eps, u.energy);	

	// Just outside of second patch
	p2.SetDirector(GenRotationMatrix(1, M_PI-1.01*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(-eps, u.energy);

	// Just inside of second patch
	p2.SetDirector(GenRotationMatrix(1, M_PI-0.99*dlim1)*nz);
	u = ff.Evaluate(p1, p2, p2.GetPosition()-p1.GetPosition(), 0);
	ASSERT_EQ(0, u.energy);
}