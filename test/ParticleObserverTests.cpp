#include "../src/Particles/Site.h"
#include "../src/Particles/ParticleObserver.h"
#include "gtest/gtest.h"

namespace SAPHRON
{
	class MockParticleObserver : public ParticleObserver
	{
		public:
			Director director;
			Position position;
			std::string species;

			MockParticleObserver() : director({ 0.0, 0.0, 0.0 }), position({ 0.0, 0.0, 0.0 }), species("foo") {}

			virtual void Update(const ParticleEvent& pEvent) override
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
	ASSERT_EQ(observer.director, s1.GetDirector());
	s1.SetPosition({ 3.0, 2.0, 1.0 });
	ASSERT_EQ(observer.position, s1.GetPosition());
	s1.SetSpecies("L2");
	ASSERT_EQ(observer.species, s1.GetSpecies());
}