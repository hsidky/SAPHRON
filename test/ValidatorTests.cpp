#include "../src/Validator/StringRequirement.h"
#include "gtest/gtest.h"

using namespace Json;

// Json schema tests from spacetelescope.github.io
TEST(StringRequirement, DefaultBehavior)
{
	StringRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;
	
	// Parse schema.	
	reader.parse("{ \"type\": \"string\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("\"This is a string\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 2.
	reader.parse("\"Déjà vu\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 3.
	reader.parse("\"\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 4.
	reader.parse("\"42\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 5.
	reader.parse("42", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.Reset();

	// Parse schema.
	reader.parse("{\"type\": \"string\",\"minLength\": 2,\"maxLength\": 3}", schema);
	validator.Parse(schema, "");

	// Input 6.
	reader.parse("\"A\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 7.
	reader.parse("\"AB\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 8.
	reader.parse("\"ABC\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 9.
	reader.parse("\"ABCD\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.Reset();

	// Parse schema.
	reader.parse("{\"type\": \"string\",\"pattern\": \"^(\\\\([0-9]{3}\\\\))?[0-9]{3}-[0-9]{4}$\"}", schema);
	validator.Parse(schema, "");

	// Input 10.
	reader.parse("\"555-1212\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 11.
	reader.parse("\"(888)555-1212\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 12.
	reader.parse("\"(888)555-1212 ext. 532\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 13.
	reader.parse("\"(800)FLOWERS\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}
