#include "../src/ForceFields/DebyeHuckelFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Particles/Particle.h"

#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(DebyeHuckelFF, DefaultBehavior)
{
	DebyeHuckelFF ff(0.001, {10.0});
	
	Particle* s1 = new Particle({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	s1->SetCharge(true);
	Particle* s2 = new Particle({1.5, 1.5, 0.0}, {0.0, 0.0, 0.0}, "L2");
	Particle* s3 = new Particle({3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	s3->SetCharge(true);
	
	Particle* s4 = new Particle({0.0, 0.0, 2.0}, {0.0, 0.0, 0.0}, "L1");
	s4->SetCharge(true);
	Particle* s5 = new Particle({1.5, 1.5, 2.0}, {0.0, 0.0, 0.0}, "L2");
	s5->SetCharge(true);
	Particle* s6 = new Particle({3.0, 0.0, 2.0}, {0.0, 0.0, 0.0}, "L3");
	s6->SetCharge(true);
	

	s1->AddBondedNeighbor(s2);
	s2->AddBondedNeighbor(s1);
	s2->AddBondedNeighbor(s3);
	s3->AddBondedNeighbor(s2);
	
	s4->AddBondedNeighbor(s5);
	s5->AddBondedNeighbor(s4);
	s5->AddBondedNeighbor(s6);
	s6->AddBondedNeighbor(s5);

	s1->AddNeighbor(s4);
	s1->AddNeighbor(s5);
	s1->AddNeighbor(s6);

	s2->AddNeighbor(s4);
	s2->AddNeighbor(s5);
	s2->AddNeighbor(s6);

	s3->AddNeighbor(s4);
	s3->AddNeighbor(s5);
	s3->AddNeighbor(s6);

	s4->AddNeighbor(s1);
	s4->AddNeighbor(s2);
	s4->AddNeighbor(s3);
	
	s5->AddNeighbor(s1);
	s5->AddNeighbor(s2);
	s5->AddNeighbor(s3);
	
	s6->AddNeighbor(s1);
	s6->AddNeighbor(s2);
	s6->AddNeighbor(s3);

	Particle m("M1");

	Particle m2("M2");

	m.AddNeighbor(&m2);
	m.AddChild(s1);
	m.AddChild(s2);
	m.AddChild(s3);

	m2.AddNeighbor(&m);
	m2.AddChild(s4);
	m2.AddChild(s5);
	m2.AddChild(s6);


	ForceFieldManager ffm;

	ffm.SetElectrostaticForcefield(ff);
	
	//Check simple atom to atom
	auto Energy = ff.Evaluate(*s1,*s3,s1->GetPosition()-s3->GetPosition(), 0);
	ASSERT_NEAR(0.33233, Energy.energy, 1e-3);

	// Validate molecule test
	auto NB = ffm.EvaluateEnergy(m);
	ASSERT_NEAR(1.4593501512, NB.energy.interelectrostatic, 1e-3);
	ASSERT_NEAR(0.3323348318, NB.energy.intraelectrostatic,1e-3);
	ASSERT_EQ(0, NB.energy.intervdw);
	ASSERT_EQ(0, NB.energy.intravdw);

}

