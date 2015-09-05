#include "../src/Moves/MoveManager.h"
#include "../src/Moves/FlipSpinMove.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(MoveManager, DefaultBehavior)
{
	// Create a few dummy moves.
	FlipSpinMove move1;
	FlipSpinMove move2;
	FlipSpinMove move3;
	FlipSpinMove move4;

	// Create move manager.
	MoveManager mm;

	// Add the moves with a specified probability. 
	mm.AddMove(&move1, 50);
	mm.AddMove(&move2, 100);
	mm.AddMove(&move3, 50);
	mm.AddMove(&move4, 200);
	ASSERT_EQ(4, mm.GetMoveCount());

	mm.RemoveMove(&move4);
	ASSERT_EQ(3, mm.GetMoveCount());
	// Nothing should happen here.
	mm.RemoveMove(&move4);
	ASSERT_EQ(3, mm.GetMoveCount());

	// Re-add.
	mm.AddMove(&move4, 200);
	ASSERT_EQ(4, mm.GetMoveCount());

	// Accumulate probabilities. 
	std::map<Move*, int> prob;
	Move* m1 = static_cast<Move*>(&move1);
	Move* m2 = static_cast<Move*>(&move2);
	Move* m3 = static_cast<Move*>(&move3);
	Move* m4 = static_cast<Move*>(&move4);
	prob[m1] = 0;
	prob[m2] = 0;
	prob[m3] = 0;
	prob[m4] = 0;

	// Collect statistics. Law of large numbers...
	for (int i = 0; i < 20000000; ++i)
	{
		Move* move = mm.SelectRandomMove();
		++prob.at(move);
	}

	double sum = 0;
	for(auto& p : prob)
		sum += p.second;

	ASSERT_NEAR(0.125, prob[m1]/sum, 1e-4);
	ASSERT_NEAR(0.250, prob[m2]/sum, 1e-4);
	ASSERT_NEAR(0.125, prob[m3]/sum, 1e-4);
	ASSERT_NEAR(0.500, prob[m4]/sum, 1e-4);
}