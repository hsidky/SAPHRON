#pragma once 

namespace SAPHRON
{
	struct Pressure
	{
		double ideal = 0;
		double pxx = 0;
		double pxy = 0;
		double pxz = 0;
		double pyy = 0;
		double pyz = 0;
		double pzz = 0;

		// Calculate isotropic pressure.
		double isotropic()
		{
			return (pxx + pyy + pzz)/3.0 + ideal;
		}

		inline Pressure& operator+=(const Pressure& rhs)
		{
			pxx += rhs.pxx;
			pxy += rhs.pxy;
			pxz += rhs.pxz;
			pyy += rhs.pyy;
			pyz += rhs.pyz;
			pzz += rhs.pzz;
			ideal += rhs.ideal;
			return *this;
		}

		inline Pressure& operator-=(const Pressure& rhs)
		{
			pxx -= rhs.pxx;
			pxy -= rhs.pxy;
			pxz -= rhs.pxz;
			pyy -= rhs.pyy;
			pyz -= rhs.pyz;
			pzz -= rhs.pzz;
			ideal -= rhs.ideal;
			return *this;
		}

		inline Pressure& operator*=(const double& rhs)
		{
			pxx *= rhs;
			pxy *= rhs;
			pxz *= rhs;
			pyy *= rhs;
			pyz *= rhs;
			pzz *= rhs;
			ideal *= rhs;
			return *this;
		}

		inline Pressure& operator/=(const double& rhs)
		{
			pxx /= rhs;
			pxy /= rhs;
			pxz /= rhs;
			pyy /= rhs;
			pyz /= rhs;
			pzz /= rhs;
			ideal /= rhs;
			return *this;
		}
	};

	inline Pressure operator+(Pressure lhs, const Pressure& rhs)
	{
		lhs.pxx += rhs.pxx;
		lhs.pxy += rhs.pxy;
		lhs.pxz += rhs.pxz;
		lhs.pyy += rhs.pyy;
		lhs.pyz += rhs.pyz;
		lhs.pzz += rhs.pzz;
		lhs.ideal += rhs.ideal;
		return lhs;
	}

	inline Pressure operator-(Pressure lhs, const Pressure& rhs)
	{
		lhs.pxx -= rhs.pxx;
		lhs.pxy -= rhs.pxy;
		lhs.pxz -= rhs.pxz;
		lhs.pyy -= rhs.pyy;
		lhs.pyz -= rhs.pyz;
		lhs.pzz -= rhs.pzz;
		lhs.ideal -= rhs.ideal;
		return lhs;
	}
}