#include "../src/Connectivities/DLSAConnectivity.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleWorld.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(DLSAConnectivity, DefaultBehavior)
{
	SimpleWorld world(10, 10, 10, 1);
	Site site1({ 0, 0, 0 }, { 1.0, 0, 0 }, "E1");
	world.ConfigureParticles({ &site1 }, { 1.0 });

	// Initialize connectivity.
	DLSAConnectivity connectivity(world, 1.0, [](Particle*, Director& dir){
		dir[0] = 1.0;
		dir[1] = 0.0;
		dir[2] = 0.0;
	},
	[](const Particle* p){
		return (int)p->GetPositionRef().x;
	});

	ForceFieldManager ffm;
	// Loop though, add this connectivity to all sites and evaluate energy.
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		particle->AddConnectivity(&connectivity);
		particle->AddObserver(&connectivity);
		ASSERT_EQ(-1.0, ffm.EvaluateHamiltonian(*particle));
	}

	// Loop though, change directors.
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		particle->SetDirector({0.0, 0.0, 1.0});
		//ffm.EvaluateHamiltonian(*particle);
	}

	// Re-evaluate energies 
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		ASSERT_EQ(0.5, ffm.EvaluateHamiltonian(*particle));
	}
}