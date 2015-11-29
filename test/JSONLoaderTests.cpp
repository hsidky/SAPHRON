#include "../src/JSON/JSONLoader.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(JSONLoader, IncludePlugin)
{
	JSONLoader loader;
	Json::Value json;
	ASSERT_NO_THROW(json = loader.LoadFile("../test/loader_test_1.json"));

	std::string expected = 
	"	{ "
	"\"imported\" : "
	"{"
	"	\"foo\" : \"bar\","
	"	\"foo2\" : \"bar\""
	"},"
	"\"test\" : \"me\""
	"}";

	Json::Reader reader;
	Json::Value root;
	reader.parse(expected, root);

	ASSERT_EQ(root, json);
}