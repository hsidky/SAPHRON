#include "../src/DensityOfStates/ElasticCoeffOP.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(ElasticCoeffOP, DefaultBehavior)
{
	int n = 25;
	// Initialize world.
	SimpleWorld world(n, n, n, 1);
	Site site1({0, 0, 0}, {0, 0, 1.0}, "E1");
	world.ConfigureParticles({&site1}, {1.0});
	world.UpdateNeighborList();

	// Initialize ElasticCoeffOP 
	int middle = ceil(n/2);
	ElasticCoeffOP op(world, 1.0, n - middle, [=](const Particle* p){
		auto& pos = p->GetPositionRef();
		return pos.x == middle; 
	});

	ASSERT_EQ(0, op.EvaluateParameter(0));

	// Register event handler for particles of interest.
	//int idx = 0; // Get index of one of the middle spins for later.
	int count = 0; // Count the number of particles that are in the "middle".
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);
		auto& pos = p->GetPositionRef();
		if(pos.x == middle)
		{
			p->AddObserver(&op);
			++count;
			//idx = i;
		}

		// Modify angle of particle.
		p->SetDirector({0, 1.0, 0.0});
	}

	ASSERT_EQ(n*n, count);

	// Re-evaluate OP.
	ASSERT_EQ(1.0/(n-middle), op.EvaluateParameter(0));

	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);

		// Modify angle of particle.
		p->SetDirector({0, sqrt(2.0)/2.0, sqrt(2.0)/2.0});
		auto eigvec = op.GetDirector();

		// Check eigenvectors are normalized.
		ASSERT_NEAR(1.0, sqrt(eigvec[0]*eigvec[0] + eigvec[1]*eigvec[1] + eigvec[2]*eigvec[2]), 1e-5);
	}

	// Re-evaluate OP.
	ASSERT_NEAR(sqrt(2.0)/(2.0*(n-middle)), op.EvaluateParameter(0), 1e-9);
}