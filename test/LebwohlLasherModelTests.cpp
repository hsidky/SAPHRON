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
	ASSERT_EQ(3.1, m.GetInteractionParameter(2,1));
	ASSERT_EQ(2.0, m.GetInteractionParameter(1,1));

	// Check default interaction parameter.
	ASSERT_EQ(0.0, m.GetIsotropicParameter());

	// Change default parameter
	ASSERT_EQ(2.0, m.SetIsotropicParameter(2.0, 1, 1));

	// Add new parameter
	ASSERT_EQ(3.1, m.SetIsotropicParameter(3.1, 1, 2));
	ASSERT_EQ(7.7, m.SetIsotropicParameter(7.7, 2, 2));
	ASSERT_EQ(3.1, m.GetIsotropicParameter(1,2));
	ASSERT_EQ(3.1, m.GetIsotropicParameter(2,1));
	ASSERT_EQ(2.0, m.GetIsotropicParameter(1,1));
	ASSERT_EQ(7.7, m.GetIsotropicParameter(2,2));

	// More cases
	double gaa = 1.0;
	double gbb = 1.0;
	double ebb = 1.0;
	double eaa = 0.4;

	// Anisotropic interactions.
	ASSERT_EQ(eaa, m.SetInteractionParameter(eaa, 1, 1));
	ASSERT_EQ(sqrt(eaa*ebb), m.SetInteractionParameter(sqrt(eaa*ebb), 1, 2));
	ASSERT_EQ(ebb, m.SetInteractionParameter(ebb, 2, 2));

	// Isotropic interactions.
	ASSERT_EQ(gaa, m.SetIsotropicParameter(gaa, 1, 1));
	ASSERT_EQ(sqrt(gaa*gbb), m.SetIsotropicParameter(sqrt(gaa*gbb), 1, 2));
	ASSERT_EQ(gbb, m.SetIsotropicParameter(gbb, 2, 2));

	// Recheck
	ASSERT_EQ(eaa, m.GetInteractionParameter(1,1));
	ASSERT_EQ(sqrt(eaa*ebb), m.GetInteractionParameter(1,2));
	ASSERT_EQ(sqrt(eaa*ebb), m.GetInteractionParameter(2,1));
	ASSERT_EQ(ebb, m.GetInteractionParameter(2,2));

	ASSERT_EQ(gaa, m.GetIsotropicParameter(1,1));
	ASSERT_EQ(sqrt(gaa*gbb), m.GetIsotropicParameter(1,2));
	ASSERT_EQ(sqrt(gaa*gbb), m.GetIsotropicParameter(2,1));
	ASSERT_EQ(gbb, m.GetIsotropicParameter(2,2));

	// Set something way out there.
	//ASSERT_EQ(5.5, m.SetInteractionParameter(5.5, 3, 5));
}

// Test the mixture setup.
TEST(LebwohlLasherModel, MixtureSetup)
{
	LebwohlLasherModel m(37, 37, 37, 1);

	// Check initial compositions
	double x  = 0;
	for(int i = 0; i < m.GetSiteCount(); i++)
	{
		auto* site = m.SelectSite(i);
		if(site->GetSpecies() == 1)
			x += 1.0;
	}

	ASSERT_DOUBLE_EQ(1.0, x/m.GetSiteCount());

	std::vector<double> target {0.5, 0.4, 0.1};
	// Set target mole fractions for species
	m.ConfigureMixture(3, target);

	// Check final compositions.
	std::vector<double> actual {0.0, 0.0, 0.0};
	int n = m.GetSiteCount();
	for(int i = 0; i < n; i++)
	{
		auto* site = m.SelectSite(i);
		actual[site->GetSpecies()-1]++;
	}

	ASSERT_NEAR(target[0], actual[0]/n, 1e-3);
	ASSERT_NEAR(target[1], actual[1]/n, 1e-3);
	ASSERT_NEAR(target[2], actual[2]/n, 1e-3);
}
