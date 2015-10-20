#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Site, DefaultConstructor)
{
	Site s({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	auto pos = s.GetPosition();
	ASSERT_EQ(0.0, pos[0]);
	ASSERT_EQ(0.0, pos[1]);
	ASSERT_EQ(0.0, pos[2]);

	Director d1 {0.0, 0.0, 0.0};
	ASSERT_TRUE(is_close(d1, s.GetDirector(), 1e-11));
	ASSERT_EQ("L1", s.GetSpecies());

	// Set a few things
	Director d {0.1, 0.2, 0.4};
	s.SetDirector(d);
	ASSERT_TRUE(is_close(d, s.GetDirector(), 1e-11));
	d[0] = 1.0;
	ASSERT_FALSE(is_close(d, s.GetDirector(), 1e-11));

	// Test checkpoint 
	s.SetCheckpoint();
	ASSERT_TRUE(is_close(Position({0.0, 0.0, 0.0}), s.GetCheckpoint(),1e-11));
	s.SetPosition({2.0, 0, 0});
	ASSERT_TRUE(is_close(Position({2.0, 0.0, 0.0}), s.GetCheckpointDist(),1e-11));
	ASSERT_TRUE(is_close(Position({0.0, 0.0, 0.0}), s.GetCheckpoint(),1e-11));
	ASSERT_EQ(2.0, arma::norm(s.GetCheckpoint() - s.GetPosition()));
	s.SetCheckpoint();
	ASSERT_TRUE(is_close(Position({2.0, 0.0, 0.0}), s.GetCheckpoint(),1e-11));
}

TEST(Site, Identifiers)
{
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	ASSERT_EQ("L1", s1.GetSpecies());
	ASSERT_EQ(0, s1.GetSpeciesID());
	ASSERT_EQ(2, s1.GetGlobalIdentifier());

	Site s2({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	ASSERT_EQ("L1", s2.GetSpecies());
	ASSERT_EQ(0, s2.GetSpeciesID());
	ASSERT_EQ(3, s2.GetGlobalIdentifier());

	Director dir{4.0, 5.0, 6.0};
	Position pos{1.0, 2.0, 3.0};

	Site s3({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, "L2");

	ASSERT_EQ("L2", s3.GetSpecies());
	ASSERT_EQ(1, s3.GetSpeciesID());
	ASSERT_EQ(4, s3.GetGlobalIdentifier());

	auto list = Particle::GetSpeciesList();
	ASSERT_EQ(2, (int)list.size());

	// Test copy 
	Particle* particle = s3.Clone();

	ASSERT_TRUE(is_close(dir, particle->GetDirector(), 1e-11));
	ASSERT_TRUE(is_close(pos, particle->GetPosition(),1e-11));
	ASSERT_EQ(5, particle->GetGlobalIdentifier());

	// Test setting new identifier.
	ASSERT_EQ(5, particle->SetGlobalIdentifier(5));
	ASSERT_EQ(5, particle->GetGlobalIdentifier());
	ASSERT_EQ(3, s2.SetGlobalIdentifier(3));
	ASSERT_EQ(3, s2.GetGlobalIdentifier());

	// Test changing identifier. It should give 6 which is the next free index.
	ASSERT_EQ(6, s2.SetGlobalIdentifier(5));

	// There should only be 4 particles in the map because items are cleared 
	// as they are destroyed.
	ASSERT_EQ(4, (int)Particle::GetParticleMap().size());

	delete particle;
}

TEST(Site, Neighbors)
{
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L2");
	Site s3({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L3");
	Site s4({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L4");

	// Add neighbors.
	s1.AddNeighbor(&s2);
	s1.AddNeighbor(&s3);
	s1.AddNeighbor(&s4);

	// Verify
	std::vector<std::string> vals = {"L2", "L3", "L4"};
	auto& neighbors = s1.GetNeighbors();
	int i = 0;
	for(auto& neighbor: neighbors)
	{
		auto id = neighbor->GetSpecies();

		ASSERT_EQ(vals[i], id);
		i++;
	}

	// Check that neighbor destructor works. 
	Site* s5 = new Site({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L5");
	s1.AddNeighbor(s5);
	s5->AddNeighbor(&s1);

	ASSERT_EQ(4, (int)s1.GetNeighbors().size());
	ASSERT_EQ(1, (int)s5->GetNeighbors().size());

	// Dlete 5. Make sure it's all been cleaned up. 
	delete s5; 

	ASSERT_EQ(3, (int)s1.GetNeighbors().size());
}

TEST(Site, PositionArithmetic)
{
	Site s1({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({1.0, 1.0, 2.0}, {0.0, 0.0, 0.0}, "L2");
	Site s3({1.0, 2.0, 1.0}, {0.0, 0.0, 0.0}, "L3");
	Site s4({2.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, "L4");

	Position p1({2.0, 2.0, 3.0});
	Position p2({2.0, 3.0, 2.0});
	Position p3({3.0, 2.0, 2.0});
	ASSERT_TRUE(is_close(p1, s1.GetPosition() + s2.GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(p2, s1.GetPosition() + s3.GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(p3, s1.GetPosition() + s4.GetPosition(),1e-11)); 
	ASSERT_EQ(sqrt(17.0), arma::norm(s1.GetPosition() + s2.GetPosition()));
	ASSERT_EQ(sqrt(17.0), arma::norm(s1.GetPosition() + s3.GetPosition()));
	ASSERT_EQ(sqrt(17.0), arma::norm(s1.GetPosition() + s4.GetPosition()));

	Position p4({0.0, 0.0, -1.0});
	Position p5({0.0, -1.0, 0.0});
	Position p6({-1.0, 0.0, 0.0});
	ASSERT_TRUE(is_close(p4, s1.GetPosition() - s2.GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(p5, s1.GetPosition() - s3.GetPosition(),1e-11)); 
	ASSERT_TRUE(is_close(p6, s1.GetPosition() - s4.GetPosition(),1e-11)); 
	ASSERT_EQ(1.0, arma::norm(s1.GetPosition() - s2.GetPosition()));
	ASSERT_EQ(1.0, arma::norm(s1.GetPosition() - s3.GetPosition()));
	ASSERT_EQ(1.0, arma::norm(s1.GetPosition() - s4.GetPosition()));
}