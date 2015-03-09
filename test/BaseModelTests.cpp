#include "MockBaseModel.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <vector>

using namespace Models;

// Test the default constructor of BaseModel base class.
TEST(BaseModel, DefaultConstructor)
{
	int num = 1000;

	// Initialize base model with 1000 sites.
	MockBaseModel b(num, 1);

	ASSERT_EQ(num, b.GetSiteCount());
}

// Test BaseModel cloning.
TEST(BaseModel, Cloning)
{
	int n = 1000;
	// create regular mock model.
	MockBaseModel m1(n, 1);

	// Check sites
	ASSERT_EQ(n, m1.GetSiteCount());

	// Clone object cast into baseclass.
	BaseModel* m2 = m1.Clone();

	// Check test prop
	ASSERT_EQ(n, m2->GetSiteCount());

	// Add a site to m2 and check m1 & m2 for consistency.
	m2->AddSite();
	ASSERT_EQ(n+1, m2->GetSiteCount());
	ASSERT_EQ(n, m1.GetSiteCount());

	delete m2;
}

// Test random site index range is correct and sampling probability is as expected.
TEST(BaseModel, RandomSiteIndex)
{
	int n = 10000;
	int guesses = 1000000;

	MockBaseModel b(n, 1);

	// Array containing indices of sites. We will test to see if #1 the random
	// site index falls within the correct range (0->N-1) and that all sites are
	// at some point chosen.
	std::vector<int> ci (n, 0);

	int site = b.GetRandomSiteIndex();
	for(int i = 0; i < guesses; i++)
	{
		ASSERT_GE(site, 0);
		ASSERT_LT(site, n);
		ci[site] = 1;
		site = b.GetRandomSiteIndex();
	}

	ASSERT_EQ(0, std::count(ci.begin(), ci.end(), 0));
}

// Test that base model returns a proper reference and not a cloned object.
TEST(BaseModel, DrawSample)
{
	MockBaseModel b(10, 1);

	auto site = b.DrawSample();

	// Make unique change to referenced site.
	site->SetZUnitVector(-3.0);

	int found = 0;
	for(int i = 0; i < b.GetSiteCount(); i++)
	{
		auto select = b.SelectSite(i);
		if(select->GetZUnitVector() == -3.0)
			found = 1;
	}

	ASSERT_EQ(1, found);
}

// Test site locations.
TEST(BaseModel, SiteLocations)
{
	MockBaseModel b(20, 1);

	for (size_t i = 0; i < b.GetSiteCount(); i++)
		ASSERT_EQ(i, b.SelectSite(i)->GetLocation());

	// Add a site. Re-check.
	b.AddSite();

	for (size_t i = 0; i < b.GetSiteCount(); i++)
		ASSERT_EQ(i, b.SelectSite(i)->GetLocation());
}
