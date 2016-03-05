#pragma once 

#include <armadillo>

namespace SAPHRON
{
	// Typedefs from arma.
	typedef arma::vec::fixed<3> Vector3D;
	typedef arma::mat33 Matrix3D;
	typedef Vector3D Director;
	typedef Vector3D Position;

	// Function to identify if two vecctors are close.
	inline bool is_close(const Vector3D& x, const Vector3D& y, double tol)
	{
		bool close(false);
		if(arma::norm(x-y, "inf") < tol)
			close  = true;

		return close;
	}

	// Fast norm.
	template<typename T>
	inline double fnorm(const T& v)
	{
		return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	}

	// Fast dot.
	template<typename T1, typename T2>
	inline double fdot(const T1& v1, const T2& v2)
	{
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	}
}