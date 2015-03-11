#pragma once

#include "../src/Models/BaseModel.h"

using namespace Models;

class MockBaseModel : public BaseModel
{
	public:
#ifdef _MSC_VER
		MockBaseModel(int size, int seed = 1) : BaseModel(size, seed){}
#else
		using BaseModel::BaseModel;
#endif
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
