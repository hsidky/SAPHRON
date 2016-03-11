#include "../src/DensityOfStates/ElasticCoeffOP.h"
#include "../src/Particles/Particle.h"
#include "../src/Worlds/World.h"
#include "../src/Utils/Histogram.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace SAPHRON;

TEST(ElasticCoeffOP, Dynamics)
{
	auto l = 10.;
	World world(l, l, l, 1.0, 1.0);
	Particle* site1 = new Particle({l/2., 0.0, 0.0}, {0.0, 0.0, 1.0}, "E1");
	Particle* site2 = new Particle({l/2., 0.0, 10.0}, {0.0, 0.0, 1.0}, "E1");
	world.AddParticle(site1);
	world.AddParticle(site2);

	ASSERT_EQ(2, world.GetParticleCount());

	Histogram hist(-0.02, 0.02, 200);
	ElasticCoeffOP op(hist, &world, l/2., {{4.0, 6.0}});

	ASSERT_EQ(0.0, op.EvaluateOrderParameter(world));

	site1->SetDirector({0.0, 1.0, 0.0});
	site2->SetDirector({0.0, 1.0, 0.0});
	ASSERT_EQ(2.0/l, op.EvaluateOrderParameter(world));

	// Move site 2 outside, change its director. Only site1 should be represented.
	site2->SetPosition({7.0, 0.0, 0.0});
	site2->SetDirector({0.0, 0.0, 1.0});
	ASSERT_EQ(2.0/l, op.EvaluateOrderParameter(world));

	// Move it back in.
	site2->SetPosition({l/2., 0.0, 10.0});
}

TEST(ElasticCoeffOP, DefaultBehavior)
{
	int n = 25;
	// Initialize world.
	World world(n, n, n, 1.0, 1.0);
	Particle site1({0, 0, 0}, {0, 0, 1.0}, "E1");
	world.PackWorld({&site1}, {1.0});

	// Initialize ElasticCoeffOP 
	Histogram hist(-0.02, 0.02, 200);
	int middle = ceil(n/2);
	ElasticCoeffOP op(hist, &world, n - middle, {{(double)middle, (double)middle}});

	ASSERT_EQ(0, op.EvaluateOrderParameter(world));

	// Register event handler for particles of interest.
	//int idx = 0; // Get index of one of the middle spins for later.
	int count = 0; // Count the number of particles that are in the "middle".
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);
		auto& pos = p->GetPosition();
		if(pos[0] == middle)
			++count;

		// Modify angle of particle.
		p->SetDirector({0, 1.0, 0.0});
	}

	ASSERT_EQ(n*n, count);

	// Re-evaluate OP.
	ASSERT_EQ(1.0/(n-middle), op.EvaluateOrderParameter(world));

	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);

		// Modify angle of particle.
		p->SetDirector({0, sqrt(2.0)/2.0, sqrt(2.0)/2.0});
		auto eigvec = op.GetDirector();

		// Check eigenvectors are normalized.
		ASSERT_NEAR(1.0, arma::norm(eigvec), 1e-5);
	}

	// Re-evaluate OP.
	ASSERT_NEAR(sqrt(2.0)/(2.0*(n-middle)), op.EvaluateOrderParameter(world), 1e-9);
}