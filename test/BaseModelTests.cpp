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

// Test helper functions.
TEST(BaseModel, UniformProbabilitySeeding)
{
	MockBaseModel b(1000, 1);

	// Test Random number generator seeding. Get the first random number with a
	// seed of 1, then change the seed, get a new random number, change seed back
	// and check that the first number from a seed of 1 hasn't changed. Repeat
	// with second seed.
	auto first_rand = b.GetRandomUniformProbability();
	b.ReseedRand(2);
	auto second_rand = b.GetRandomUniformProbability();
	b.ReseedRand(1);
	ASSERT_EQ(first_rand, b.GetRandomUniformProbability());
	b.ReseedRand(2);
	ASSERT_EQ(second_rand, b.GetRandomUniformProbability());
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
