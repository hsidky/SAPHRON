#pragma once 

namespace SAPHRON
{
	struct Pressure
	{
		double ideal;
		double pxx;
		double pxy;
		double pxz;
		double pyy;
		double pyz;
		double pzz;

		Pressure() : ideal(0), pxx(0), pxy(0), pxz(0), pyy(0), pyz(0), pzz(0) {}
		Pressure(double idealp, double pxxp, double pxyp, 
				 double pxzp, double pyyp, double pyzp, double pzzp) : 
		ideal(idealp), pxx(pxxp), pxy(pxyp), pxz(pxzp), pyy(pyyp), pyz(pyzp), pzz(pzzp) {}

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