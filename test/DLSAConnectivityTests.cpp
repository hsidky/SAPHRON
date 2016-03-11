#include "../src/Connectivities/DLSAConnectivity.h"
#include "../src/Particles/Particle.h"
#include "../src/Worlds/World.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(DLSAConnectivity, DefaultBehavior)
{
	World world(10, 10, 10, 1.0, 1.0);
	Particle site1({ 0, 0, 0 }, { 1.0, 0, 0 }, "E1");
	world.PackWorld({ &site1 }, { 1.0 });

	// Initialize connectivity.
	DLSAConnectivity connectivity(world, 1.0, [](const Particle&, Director& dir){
		dir[0] = 1.0;
		dir[1] = 0.0;
		dir[2] = 0.0;
	},
	[](const Particle& p){
		return (int)p.GetPosition()[0];
	});

	ForceFieldManager ffm;
	// Loop though, add this connectivity to all sites and evaluate energy.
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		particle->AddConnectivity(&connectivity);
		particle->AddObserver(&connectivity);
		auto H = ffm.EvaluateEnergy(*particle);
		ASSERT_EQ(-1.0, H.energy.connectivity);
	}

	// Loop though, change directors.
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		particle->SetDirector({0.0, 0.0, 1.0});
		//ffm.EvaluateEnergy(*particle);
	}

	// Re-evaluate energies 
	for (int i = 0; i < world.GetParticleCount(); ++i)
	{
		auto* particle = world.SelectParticle(i);
		auto H = ffm.EvaluateEnergy(*particle);
		ASSERT_EQ(0.5, H.energy.connectivity);
	}
}