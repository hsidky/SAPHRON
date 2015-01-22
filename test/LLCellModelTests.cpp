#include "../src/Models/LLCellModel.h"
#include "gtest/gtest.h"

using namespace Models;

// Tests the default constructor and initialization of the model.
TEST(LLCellModel, DefaultConstructor)
{
	int xl = 20, yl = 30, zl = 30;

	// Define the two vectors at the planar ends.
	std::vector<double> vBegin {0.0, 0.0, 1.0};
	std::vector<double> vEnd {0.0, 1.0, 0.0};

	LLCellModel m(xl, yl, zl, vBegin, vEnd, 1);

	// Check number of sites on lattice
	ASSERT_EQ(xl*yl*zl, m.GetSiteCount());

	// Make sure that no anchored site is ever sampled.
	for(int i = 1; i < 10*xl*yl*zl; i++)
	{
		auto site = m.DrawSample();
		ASSERT_NE(0.0, site->GetXCoordinate());
	}
}
