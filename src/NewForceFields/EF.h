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

	// Struct representing pair energy and virial tensor.
	struct EV
	{
		double vdw = 0., electro = 0.;
		Matrix3 virial = Eigen::Matrix3d::Zero();

		inline EV& operator+=(const EV& rhs)
		{
			vdw += rhs.vdw;
			electro += rhs.electro;
			virial += rhs.virial;
			return *this;
		}

		inline EV& operator-=(const EV& rhs)
		{
			vdw -= rhs.vdw;
			electro -= rhs.electro; 
			virial -= rhs.virial;
			return *this;
		}

		inline EV& operator*=(const double& rhs)
		{
			vdw *= rhs;
			electro *= rhs;
			virial *= rhs;
			return *this;
		}

		inline double energy() const
		{
			return vdw + electro;
		}
	};

	inline EV operator-(EV lhs, const EV& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	inline EV operator+(EV lhs, const EV& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	inline EV operator*(EV lhs, const double& rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	inline EV operator*(double lhs, const EV& rhs)
	{
		return rhs*lhs;
	}

	// Struct representing long range corrections for energy and pressure.
	struct EPTail
	{
		double energy = 0., pressure = 0.;

		inline EPTail& operator+=(const EPTail& rhs)
		{
			energy += rhs.energy;
			pressure += rhs.pressure;
			return *this;
		}
	};
}