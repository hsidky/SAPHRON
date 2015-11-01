#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Particles/ParticleObserver.h"
#include "../src/Worlds/World.h"
#include "gtest/gtest.h"

namespace SAPHRON
{
	class MockParticleObserver : public ParticleObserver
	{
		public:
			Director director;
			Position position;
			std::string species;
			double charge;

			MockParticleObserver() : director({ 0.0, 0.0, 0.0 }), position({ 0.0, 0.0, 0.0 }), species("foo"), charge(0) {}

			virtual void ParticleUpdate(const ParticleEvent& pEvent) override
			{
				auto* p = pEvent.GetParticle();
				if (pEvent.director)
				{
					director = p->GetDirector();
				}

				if (pEvent.position)
				{
					position = p->GetPosition();
				}

				if (pEvent.species)
				{
					species = p->GetSpecies();
				}

				if (pEvent.charge)
				{
					charge = p->GetCharge();
				}
			}
	};
}

using namespace SAPHRON;

TEST(ParticleObserver, DefaultBehavior)
{
	Site s1({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, "L1");

	// Add observer.
	MockParticleObserver observer;
	s1.AddObserver(&observer);

	// Change site properties.
	s1.SetDirector({ 1.0, 0.0, 0.0 });
	ASSERT_EQ(0, arma::norm(observer.director - s1.GetDirector()));
	s1.SetPosition({ 3.0, 2.0, 1.0 });
	ASSERT_TRUE(is_close(observer.position, s1.GetPosition(),1e-11));
	s1.SetSpecies("L2");
	ASSERT_EQ(observer.species, s1.GetSpecies());
	s1.SetCharge(1.5);
	ASSERT_EQ(observer.charge,s1.GetCharge());
}

TEST(ParticleObserver, AddRemoveChild)
{
	World sw(10.0, 10.0, 10.0, 1.0);

	Site* s1 = new Site({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "S1");
	Site* s2 = new Site({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "S1");

	Molecule* m1 = new Molecule("M1"); 
	m1->AddChild(s1);
	sw.AddParticle(m1);

	const auto& comp = sw.GetComposition();

	ASSERT_EQ(1, sw.GetParticleCount());
	ASSERT_EQ(1, sw.GetPrimitiveCount());
	ASSERT_EQ(1, comp.at(m1->GetSpeciesID()));
	ASSERT_EQ(1, comp.at(s1->GetSpeciesID()));

	// Add child and see if it gets reflected.
	m1->AddChild(s2);
	ASSERT_EQ(1, sw.GetParticleCount());
	ASSERT_EQ(2, sw.GetPrimitiveCount());
	ASSERT_EQ(1, comp.at(m1->GetSpeciesID()));
	ASSERT_EQ(2, comp.at(s1->GetSpeciesID()));

	// Remove child and see if it gets reflects.
	m1->RemoveChild(s1);
	ASSERT_EQ(1, sw.GetParticleCount());
	ASSERT_EQ(1, sw.GetPrimitiveCount());
	ASSERT_EQ(1, comp.at(m1->GetSpeciesID()));
	ASSERT_EQ(1, comp.at(s1->GetSpeciesID()));

	// Change child species and make sure it gets reflected. 
	s2->SetSpecies("S2");
	ASSERT_EQ(1, sw.GetParticleCount());
	ASSERT_EQ(1, sw.GetPrimitiveCount());
	ASSERT_EQ(1, comp.at(m1->GetSpeciesID()));
	ASSERT_EQ(0, comp.at(s1->GetSpeciesID()));
	ASSERT_EQ(1, comp.at(s2->GetSpeciesID()));

	// Remove parent and make sure it propogates.
	sw.RemoveParticle(m1);
	ASSERT_EQ(0, sw.GetParticleCount());
	ASSERT_EQ(0, sw.GetPrimitiveCount());
	ASSERT_EQ(0, comp.at(m1->GetSpeciesID()));
	ASSERT_EQ(0, comp.at(s1->GetSpeciesID()));
	ASSERT_EQ(0, comp.at(s2->GetSpeciesID()));

	delete m1;
	delete s1;
}