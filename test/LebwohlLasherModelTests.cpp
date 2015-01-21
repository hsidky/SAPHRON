#include "../src/Models/LebwohlLasherModel.h"
#include "gtest/gtest.h"

using namespace Models;

// Tests the Hamiltonian.
TEST(LebwohlLasherModel, EvaluateInitialHamiltonian)
{
	LebwohlLasherModel m(37,37,37, 1);

	// Check initial Hamiltonian values.
	for(int i = 0; i < m.GetSiteCount(); i++)
		ASSERT_EQ(-6.0, m.EvaluateHamiltonian(i));

	// Pick random site. Change spin and re-check Hamiltonian.
	auto site = m.SelectRandomSite();
	site->SetZUnitVector(-1);
	ASSERT_EQ(-6.0, m.EvaluateHamiltonian(*site));

	// Try another spin
	site->SetZUnitVector(0);
	ASSERT_EQ(3, m.EvaluateHamiltonian(*site));
}
