#include "../src/ForceFields/EwaldFF.h" 
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/SimInfo.h"
#include "../src/Particles/Particle.h"
#include "../src/Simulation/SimInfo.h"
#include "../src/Simulation/SimException.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

// Validate Ewald to NIST 
// values provided for SPCE water. See ref:
// http://www.nist.gov/mml/csd/informatics_research/spce_refcalcs.cfm
TEST(EwaldFF, NISTConfig1)
{
	// Set to real units.
	auto& sim = SimInfo::Instance();
	sim.SetUnits(real);

	// Load file (assumes we are in build folder.
	std::ifstream t("../test/nist_spce_ewald1.json");
	std::stringstream buffer;
	buffer << t.rdbuf();
	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	ASSERT_TRUE(reader.parse(buffer, root));

	// Build world.
	World* w = nullptr;
	ASSERT_NO_THROW(w = World::Build(root["worlds"][0], root["blueprints"]));
	ASSERT_NE(nullptr, w);

	ASSERT_EQ(100, w->GetParticleCount());
	ASSERT_EQ(300, w->GetPrimitiveCount());
	w->UpdateNeighborList();
	ASSERT_EQ(10.0, w->GetNeighborRadius());
	ASSERT_EQ(0.0, w->GetSkinThickness());
	auto L = w->GetHMatrix();
	ASSERT_EQ(20, L(0,0));
	ASSERT_EQ(20, L(1,1));
	ASSERT_EQ(20, L(2,2));

	// Initialize forcefields.
	std::vector<ForceField*> fflist;
	ForceFieldManager ffm;

	ASSERT_NO_THROW(ForceField::BuildForceFields(root["forcefields"], &ffm, fflist));

	auto E = ffm.EvaluateEnergy(*w);
	// Values are checked to 6 sig figs.
    // vdW dispersion.
	E.energy /= sim.GetkB();
	ASSERT_NEAR(9.95387E+04, E.energy.intervdw, 1e-1);
	ASSERT_NEAR(-8.23715E+02, E.energy.tail, 1e-3);
	ASSERT_EQ(0, E.energy.intravdw);

	// This is from Ewald.
	ASSERT_NEAR(-5.58889E+05, E.energy.interelectrostatic, 1e-1);
    ASSERT_NEAR(6.27009E+03-2.84469E+06, E.energy.electrotail, 3.0);
    ASSERT_NEAR(2.80999E+06-7.35708e+06, E.energy.intraelectrostatic, 1.0);

	delete w;
}
