#include "../src/Validator/StringRequirement.h"
#include "../src/Validator/IntegerRequirement.h"
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

TEST(IntegerRequirement, DefaultBehavior)
{
	IntegerRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;
	
	// Parse schema.	
	reader.parse("{ \"type\": \"integer\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("42", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("-1", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("3.1415926", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 4.
	reader.parse("\"42\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.	
	reader.parse("{ \"type\": \"integer\", \"multipleOf\": 2 }", schema);
	validator.Parse(schema, "");

	// Input 5.
	reader.parse("42", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 6.
	reader.parse("43", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}

TEST(NumberRequirement, DefaultBehavior)
{
	IntegerRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;
	
	// Parse schema.	
	reader.parse("{ \"type\": \"number\", \"multipleOf\": 1.0 }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("42", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("42.0", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("3.14156926", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.	
	reader.parse("{ \"type\": \"number\"}", schema);
	validator.Parse(schema, "");

	// Input 4.
	reader.parse("-1", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 5.
	reader.parse("5.0", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 6.
	reader.parse("2.99792458e8", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 7.
	reader.parse("\"42\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.	
	reader.parse("{ \"type\" : \"number\", \"multipleOf\" : 10 }", schema);
	validator.Parse(schema, "");	

	// Input 8.
	reader.parse("0", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 9.
	reader.parse("10", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 10.
	reader.parse("20", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 11.
	reader.parse("23", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.	
	reader.parse("{ \"type\": \"number\", \"minimum\": 0, \"maximum\": 100, \"exclusiveMaximum\": true }", schema);
	validator.Parse(schema, "");	

	// Input 12.
	reader.parse("-1", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 13.
	reader.parse("0", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 14.
	reader.parse("10", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 15.
	reader.parse("99", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 16.
	reader.parse("100", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 17.
	reader.parse("101", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
}