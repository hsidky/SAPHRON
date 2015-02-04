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

// Test the behavior of the interaction parameters implementation.
TEST(LebwohlLasherModel, InteractionParameters)
{
	LebwohlLasherModel m(37,37,37, 1);

	// Check default interaction parameter.
	ASSERT_EQ(1.0, m.GetInteractionParameter());

	// Change default parameter
	ASSERT_EQ(2.0, m.SetInteractionParameter(2.0, 1, 1));

	// Add new parameter
	ASSERT_EQ(3.1, m.SetInteractionParameter(3.1, 1, 2));
	ASSERT_EQ(3.1, m.GetInteractionParameter(1,2));
	ASSERT_EQ(2.0, m.GetInteractionParameter(1,1));

	// Set something way out there.
	//ASSERT_EQ(5.5, m.SetInteractionParameter(5.5, 3, 5));
}
