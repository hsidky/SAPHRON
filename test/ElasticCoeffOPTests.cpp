#include "../src/DensityOfStates/ElasticCoeffOP.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleLatticeWorld.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(ElasticCoeffOP, DefaultBehavior)
{
	int n = 25;
	// Initialize world.
	SimpleLatticeWorld world(n, n, n, 1);
	Site site1({0, 0, 0}, {0, 0, 1.0}, "E1");
	world.ConfigureParticles({&site1}, {1.0});
	world.ConfigureNeighborList();

	// Initialize ElasticCoeffOOP 
	int middle = ceil(n/2);
	ElasticCoeffOP op(world, 1.0, n - middle, [=](const Particle* p){
		auto& pos = p->GetPositionRef();
		return pos.x == middle; 
	});

	ASSERT_EQ(0, op.EvaluateParameter(0));

	// Register event handler for particles of interest.
	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);
		auto& pos = p->GetPositionRef();
		if(pos.x == middle)
			p->AddObserver(&op);

		// Modify angle of particle.
		p->SetPosition({0, 1.0, 0.0});
	}

	// Re-evaluate OP.
	ASSERT_EQ(1/(n-middle), op.EvaluateParameter(0));

	for(int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* p = world.SelectParticle(i);

		// Modify angle of particle.
		p->SetPosition({0, -1.0, 0.0});
	}

	// Re-evaluate OP.
	ASSERT_EQ(-1/(n-middle), op.EvaluateParameter(0));

}