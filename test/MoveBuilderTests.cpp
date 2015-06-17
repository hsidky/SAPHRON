#include "../src/Moves/MoveBuilder.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(MoveBuilder, DefaultBehavior)
{
	MoveBuilder builder;
	const char foo[] = "{ \"type\" : \"FlipSpin\" }";
	builder.Validate(foo);
}