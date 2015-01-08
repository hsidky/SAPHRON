#pragma once

#include "../src/Models/BaseModel.h"

using namespace Models;

class MockBaseModel : public BaseModel
{
	public:

		using BaseModel::BaseModel;

		double EvaluateHamiltonian(int site)
		{
			return site;
		}

		double EvaluateHamiltonian(Site* site)
		{
			return site->GetZUnitVector();
		}

		double AcceptanceProbability(double prevH, double currH)
		{
			return (currH-prevH);
		}
};
