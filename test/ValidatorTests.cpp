#include "../src/Validator/StringRequirement.h"
#include "../src/Validator/IntegerRequirement.h"
#include "../src/Validator/ObjectRequirement.h"
#include "../src/Validator/ArrayRequirement.h"
#include "../src/Validator/BooleanRequirement.h"
#include "../src/Validator/NullRequirement.h"
#include "../src/Validator/EnumRequirement.h"
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

	// Parse schema. 
	reader.parse("{ \"type\" : \"string\", \"enum\" : [\"foo\", \"bar\"]}", schema);
	validator.Parse(schema, "");

	// Input 14.
	reader.parse("\"foo\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 15.
	reader.parse("\"bar\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 16.
	reader.parse("\"bla\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.
	reader.parse("{\n  \"type\": \"string\",\n  "
				 "\"enum\": [\"red\", \"amber\", \"green\"]\n}\n", schema);
	validator.Parse(schema, "");

	// Input 17.
	reader.parse("\"red\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 18.
	reader.parse("\"blue\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.
	reader.parse("{\n  \"type\": \"string\",\n  "
				 "\"enum\": [\"red\", \"amber\", \"green\", null]\n}", schema);
	validator.Parse(schema, "");

	// Input 19.
	reader.parse("\"red\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 20.
	reader.parse("null", input);
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

TEST(ObjectRequirement, DefaultBehavior)
{
	ObjectRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;
	
	// Parse schema.	
	reader.parse("{ \"type\": \"object\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("{\n   \"key\"         : \"value\",\n   \"another_key\" : \"another_value\"\n}\n", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("{\n    \"Sun\"     : 1.9891e30,\n    \"Jupiter\" : 1.8986e27,\n"
				 "    \"Saturn\"  : 5.6846e26,\n    \"Neptune\" : 10.243e25,\n   "
				 " \"Uranus\"  : 8.6810e25,\n    \"Earth\"   : 5.9736e24,\n      "
				 " \"Venus\"   : 4.8685e24,\n    \"Mars\"    : 6.4185e23,\n      "
				 "\"Mercury\" : 3.3022e23,\n    \"Moon\"    : 7.349e22,\n        " 
				 "\"Pluto\"   : 1.25e22\n}\n", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("\"Not an object\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 4.
	reader.parse("[\"An\", \"array\", \"not\", \"an\", \"object\"]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Validate schema. 
	reader.parse("{\n  \"type\": \"object\",\n  \"properties\": {\n   "
				 "\"number\":      { \"type\": \"number\" },\n        "
				 "\"street_name\": { \"type\": \"string\" },\n        "
				 "\"street_type\": { \"type\": \"string\",\n          "
				 "\"enum\": [\"Street\", \"Avenue\", \"Boulevard\"]\n "
				 "}\n  }\n}\n", schema);
	validator.Parse(schema, "");

	// Input 5.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", \"street_type\": \"Avenue\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 6.
	reader.parse("{ \"number\": \"1600\", \"street_name\": \"Pennsylvania\", \"street_type\": \"Avenue\" }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 7.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();
	
	// Input 8.
	reader.parse("{ }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 9.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", "
				 " \"street_type\": \"Avenue\",\n  \"direction\": \"NW\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Parse schema.
	reader.parse("{\n  \"type\": \"object\",\n  \"properties\": {\n   "
				 "\"number\":      { \"type\": \"number\" },\n        "
				 "\"street_name\": { \"type\": \"string\" },\n        "
				 "\"street_type\": { \"type\": \"string\",\n          "
				 "\"enum\": [\"Street\", \"Avenue\", \"Boulevard\"]\n "
				 "}\n  },\n  \"additionalProperties\": false\n}\n", schema);
	validator.Parse(schema, "#");

	// Input 10.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", \"street_type\": \"Avenue\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 11.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", "
				 " \"street_type\": \"Avenue\",\n  \"direction\": \"NW\" }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Parse schema.
	reader.parse("{\n  \"type\": \"object\",\n  \"properties\": {\n   "
				 "\"number\":      { \"type\": \"number\" },\n        "
				 "\"street_name\": { \"type\": \"string\" },\n        "
				 "\"street_type\": { \"type\": \"string\",\n          "
				 "\"enum\": [\"Street\", \"Avenue\", \"Boulevard\"]\n "
				 "}\n  },\n  \"additionalProperties\": { \"type\": \"string\" }\n}\n", schema);
	validator.Parse(schema, "");

	// Input 12.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", \"street_type\": \"Avenue\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 13.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", "
				 " \"street_type\": \"Avenue\",\n  \"direction\": \"NW\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 14.
	reader.parse("{ \"number\": 1600, \"street_name\": \"Pennsylvania\", "
				 " \"street_type\": \"Avenue\",\n  \"office_number\": 201  }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema.
	reader.parse("{\n  \"type\": \"object\",\n  \"properties\": {\n "
				 "\"name\":      { \"type\": \"string\" },\n        "
				 "\"email\":     { \"type\": \"string\" },\n        "
				 "\"address\":   { \"type\": \"string\" },\n        "
				 "\"telephone\": { \"type\": \"string\" }\n  },\n   "
				 " \"required\": [\"name\", \"email\"]\n}", schema);
	validator.Parse(schema, "");

	// Input 15.
	reader.parse("{\n  \"name\": \"William Shakespeare\",\n  \"email\": \"bill@stratford-upon-avon.co.uk\"\n}\n", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 16.
	reader.parse("{\n  \"name\": \"William Shakespeare\",\n "
				 "\"email\": \"bill@stratford-upon-avon.co.uk\",\n  "
				 "\"address\": \"Henley Street, Stratford-upon-Avon, Warwickshire, England\",\n "
				 "\"authorship\": \"in question\"\n}", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 17.
	reader.parse("{\n  \"name\": \"William Shakespeare\",\n "
				 " \"address\": \"Henley Street, Stratford-upon-Avon, Warwickshire, England\",\n}", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	reader.parse("{\n  \"type\": \"object\",\n  \"minProperties\": 2,\n  \"maxProperties\": 3\n}", schema);
	validator.Parse(schema, "");

	// Input 18.
	reader.parse("{}", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 19.
	reader.parse("{ \"a\": 0 }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 20.
	reader.parse("{ \"a\": 0, \"b\": 1 }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 21.
	reader.parse("{ \"a\": 0, \"b\": 1, \"c\": 2 }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 22.
	reader.parse("{ \"a\": 0, \"b\": 1, \"c\": 2, \"d\": 3 }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema. (this validates depdendencies).
	reader.parse("{\n  \"type\": \"object\",\n\n  \"properties\": {\n "
				 "\"name\": { \"type\": \"string\" },\n    \"credit_card\": "
				 "{ \"type\": \"number\" },\n    \"billing_address\": " 
				 "{ \"type\": \"string\" }\n  },\n\n  \"required\": "
				 "[\"name\"],\n\n  \"dependencies\": {\n    \"credit_card\": "
				 "[\"billing_address\"]\n  }\n}", schema);
	validator.Parse(schema, "");

	// Input 23.
	reader.parse("{\n  \"name\": \"John Doe\",\n  \"credit_card\": "
				 "5555555555555555,\n  \"billing_address\": \"555 Debtor's Lane\"\n}", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 24.
	reader.parse("{\n  \"name\": \"John Doe\",\n  \"credit_card\": 5555555555555555\n}\n", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 25.
	reader.parse("{\n  \"name\": \"John Doe\"\n}", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 26.
	reader.parse("{\n  \"name\": \"John Doe\",\n  \"billing_address\": \"555 Debtor's Lane\"\n}\n", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Parse schema.
	reader.parse("{\n  \"type\": \"object\",\n\n  \"properties\": {\n  "
				 "\"name\": { \"type\": \"string\" },\n    \"credit_card\": "
				 "{ \"type\": \"number\" },\n    \"billing_address\": { \"type\": "
				 "\"string\" }\n  },\n\n  \"required\": [\"name\"],\n\n  \"dependencies\": "
				 "{\n    \"credit_card\": [\"billing_address\"],\n    \"billing_address\": "
				 "[\"credit_card\"]\n  }\n}\n", schema);
	validator.Parse(schema, "");
	
	// Input 27.
	reader.parse("{\n  \"name\": \"John Doe\",\n  \"credit_card\": 5555555555555555\n}\n", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 28.
	reader.parse("{\n  \"name\": \"John Doe\",\n  \"billing_address\": \"555 Debtor's Lane\"\n}", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema. Pattern properties.
	reader.parse("{\n  \"type\": \"object\",\n  \"patternProperties\": "
				 "{\n    \"^S_\": { \"type\": \"string\" },\n    \"^I_\": "
				 "{ \"type\": \"integer\" }\n  },\n  \"additionalProperties\": "
				 "false\n}\n", schema);
	validator.Parse(schema, "");

	// Input 29.
	reader.parse("{ \"S_25\": \"This is a string\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 30.
	reader.parse("{ \"I_0\": 42 }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 31.
	reader.parse("{ \"S_0\": 42 }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 32.
	reader.parse("{ \"I_42\": \"This is a string\" }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 33.
	reader.parse("{ \"keyword\": \"value\" }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	// Parse schema. 
	reader.parse("{\n  \"type\": \"object\",\n  \"properties\": "
				 " {\n    \"builtin\": { \"type\": \"number\" }\n "
				 " },\n  \"patternProperties\": {\n    \"^S_\": { "
				 " \"type\": \"string\" },\n    \"^I_\": { \"type\": "
				 " \"integer\" }\n  },\n  \"additionalProperties\": "
				 " { \"type\": \"string\" }\n}", schema);
	validator.Parse(schema, "");

	// Input 34.
	reader.parse("{ \"builtin\": 42 }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 35.
	reader.parse("{ \"keyword\": \"value\" }", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 36.
	reader.parse("{ \"keyword\": 42 }", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}

TEST(ArrayRequirement, DefaultBehavior)
{
	ArrayRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;

	// Parse schema. 
	reader.parse("{ \"type\": \"array\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("[1, 2, 3, 4, 5]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("[3, \"different\", { \"types\" : \"of values\" }]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("{\"Not\": \"an array\"}", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Parse schema.
	reader.parse("{\n  \"type\": \"array\",\n  \"items\": {\n    \"type\": \"number\"\n  }\n}", schema);
	validator.Parse(schema, "");

	// Input 4.
	reader.parse("[1, 2, 3, 4, 5]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 5.
	reader.parse("[1, 2, \"3\", 4, 5]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 6.
	reader.parse("[]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());	

	// Parse schema 
	reader.parse("{\n  \"type\": \"array\",\n  \"items\": [\n  "
				 "{\n      \"type\": \"number\"\n    },\n    {\n  "
				 "\"type\": \"string\"\n    },\n    {\n      \"type\": \"string\",\n "
				 "\"enum\": [\"Street\", \"Avenue\", \"Boulevard\"]\n    },\n    {\n "
				 "\"type\": \"string\",\n      \"enum\": [\"NW\", \"NE\", \"SW\", \"SE\"]\n "
				 "   }\n  ]\n}\n", schema);
	validator.Parse(schema, "");

	// Input 7.
	reader.parse("[1600, \"Pennsylvania\", \"Avenue\", \"NW\"]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 8.
	reader.parse("[24, \"Sussex\", \"Drive\"]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 9.
	reader.parse("[\"Palais de l'Élysée\"]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 10.
	reader.parse("[10, \"Downing\", \"Street\"]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 11.
	reader.parse("[1600, \"Pennsylvania\", \"Avenue\", \"NW\", \"Washington\"]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	// Parse schema 
	reader.parse("{\n  \"type\": \"array\",\n  \"items\": [\n  "
				 "{\n      \"type\": \"number\"\n    },\n    {\n  "
				 "\"type\": \"string\"\n    },\n    {\n      \"type\": \"string\",\n "
				 "\"enum\": [\"Street\", \"Avenue\", \"Boulevard\"]\n    },\n    {\n "
				 "\"type\": \"string\",\n      \"enum\": [\"NW\", \"NE\", \"SW\", \"SE\"]\n "
				 "   }\n  ],\"additionalItems\": false}\n", schema);
	validator.Parse(schema, "");

	// Input 12.
	reader.parse("[1600, \"Pennsylvania\", \"Avenue\", \"NW\"]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 13.
	reader.parse("[1600, \"Pennsylvania\", \"Avenue\"]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 14.
	reader.parse("[1600, \"Pennsylvania\", \"Avenue\", \"NW\", \"Washington\"]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());

	reader.parse("{\n  \"type\": \"array\",\n  \"minItems\": 2,\n  \"maxItems\": 3\n}", schema);
	validator.Parse(schema, "");

	// Input 15.
	reader.parse("[]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 16.
	reader.parse("[1]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 17.
	reader.parse("[1, 2]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 18.
	reader.parse("[1, 2, 3]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	// Input 19.
	reader.parse("[1, 2, 3, 4]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();		

	reader.parse("{\n  \"type\": \"array\",\n  \"uniqueItems\": true\n}", schema);
	validator.Parse(schema, "");

	// Input 20.
	reader.parse("[1, 2, 3, 4, 5]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 21.
	reader.parse("[1, 2, 3, 3, 4]", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();	

	// Input 22.
	reader.parse("[]", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
}

TEST(BooleanRequirement, DefaultBehavior)
{
	BooleanRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;

	// Parse schema.
	reader.parse("{ \"type\": \"boolean\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("true", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("false", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("\"true\"", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 4.
	reader.parse("0", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}

TEST(NullRequirement, DefaultBehavior)
{
	NullRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;

	// Parse schema.
	reader.parse("{ \"type\": \"null\" }", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("null", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("false", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("0", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
	
	validator.ClearErrors();
	validator.ClearNotices();

	// Input 4.
	reader.parse("", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}

TEST(EnumRequirement, DefaultBehavior)
{
	EnumRequirement validator;
	Json::Value schema;
	Json::Value input;
	Reader reader;

	reader.parse("[\"red\", \"amber\", \"green\", null, 42]", schema);
	validator.Parse(schema, "");

	// Input 1.
	reader.parse("\"red\"", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 2.
	reader.parse("null", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 3.
	reader.parse("42", input);
	validator.Validate(input, "");
	ASSERT_FALSE(validator.HasErrors());

	validator.ClearErrors();
	validator.ClearNotices();

	// Input 4.
	reader.parse("0", input);
	validator.Validate(input, "");
	ASSERT_TRUE(validator.HasErrors());
}