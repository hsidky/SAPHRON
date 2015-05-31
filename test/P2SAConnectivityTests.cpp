#include "../src/Connectivities/P2SAConnectivity.h"
#include "../src/Particles/Site.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(P2SAConnectivity, DefaultBehavior)
{
	// Test pure connectivity on a site.
	P2SAConnectivity connectivity(1.0, {1.0, 0, 0});

	Site site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	site1.AddConnectivity(&connectivity);

	ForceFieldManager ffm;

	ASSERT_EQ(-1.0, ffm.EvaluateHamiltonian(site1).connectivity);
}