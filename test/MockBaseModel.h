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

		double EvaluateHamiltonian(Site& site)
		{
			return site.GetZUnitVector();
		}

		// Clone model.
		virtual BaseModel* Clone() const
		{
			return new MockBaseModel(static_cast<const MockBaseModel&>(*this));
		}
};
