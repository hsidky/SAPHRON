#pragma once 

#include "../Utils/Types.h"

namespace SAPHRON
{
	// Struct representing energy, force tuple.
	struct EF
	{
		double energy;
		Vector3 force;
	};

	// Struct representing energy and virial tensor.
	struct EP
	{
		double intervdw = 0.;
		Matrix3 virial = Eigen::Matrix3d::Zero();

		inline EP& operator+=(const EP& rhs)
		{
			intervdw += rhs.intervdw;
			virial += rhs.virial;
			return *this;
		}
	};
}