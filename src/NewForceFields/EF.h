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

	// Struct representing long range corrections for energy and pressure.
	struct EPTail
	{
		double etail = 0., ptail = 0.;

		inline EPTail& operator+=(const EPTail& rhs)
		{
			etail += rhs.etail;
			ptail += rhs.ptail;
			return *this;
		}
	};
}