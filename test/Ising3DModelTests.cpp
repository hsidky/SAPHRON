#include "../src/Models/Ising3DModel.h"
#include "gtest/gtest.h"

using namespace Models;

// Tests the default constructor and initialization of the Ising 3D model.
TEST(Ising3DModel, DefaultConstructor)
{
	int n = 37;
	Ising3DModel m(n, 1);

	// Check the number of sites on the lattice
	ASSERT_EQ(n * n * n, m.GetSiteCount());

	// Check that the coordinates were properly initialized.
	int i = 0;
	for(int x = 1; x <= n; x++)
		for(int y = 1; y <= n; y++)
			for(int z = 1; z <= n; z++)
			{
				auto site = m.SelectSite(i);

				// Check coordinates
				ASSERT_EQ(x, site->GetXCoordinate());
				ASSERT_EQ(y, site->GetYCoordinate());
				ASSERT_EQ(z, site->GetZCoordinate());
				i++;

				// Check neighbors
				std::vector<double> n1 =
				{(x == n) ? 1.0 : (double) x + 1.0, (double) y, (double) z};
				std::vector<double> n2 =
				{(x == 1) ? (double) n : x - 1.0, (double) y, (double) z};
				std::vector<double> n3 =
				{(double) x, (y == n) ? 1.0 : (double) y + 1.0, (double) z};
				std::vector<double> n4 =
				{(double) x, (y == 1) ? (double) n : y - 1.0, (double) z};
				std::vector<double> n5 =
				{(double) x, (double) y, (z == n) ? 1.0 : (double) z + 1.0};
				std::vector<double> n6 =
				{(double) x, (double) y, (z == 1) ? (double) n : z - 1.0};

				auto neighbors = site->GetNeighbors();
				ASSERT_EQ(6, neighbors.size());

				// Verify neighbors are one of 6 possibilities.
				for(auto &nindex : neighbors)
				{
					auto neighbor = m.SelectSite(nindex)->GetCoordinates();
					ASSERT_TRUE(
					        neighbor == n1 ||
					        neighbor == n2 ||
					        neighbor == n3 ||
					        neighbor == n4 ||
					        neighbor == n5 ||
					        neighbor == n6
					        );
				}

				// Check initial unit vectors
				ASSERT_EQ(1, site->GetZUnitVector());
				ASSERT_EQ(0, site->GetXUnitVector());
				ASSERT_EQ(0, site->GetYUnitVector());
			}
}

// Tests the Hamiltonian.
TEST(Ising3DModel, EvaluateInitialHamiltonian)
{
	Ising3DModel m(37, 1);

	// Check initial Hamiltonian values.
	for(int i = 0; i < m.GetSiteCount(); i++)
		ASSERT_EQ(-6, m.EvaluateHamiltonian(i));

	// Pick random site. Change spin and re-check Hamiltonian.
	auto site = m.SelectRandomSite();
	site->SetZUnitVector(-1);
	ASSERT_EQ(6, m.EvaluateHamiltonian(*site));
}
