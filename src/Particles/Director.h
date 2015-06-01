#pragma once 

#include <cmath>
#include <vector>

namespace SAPHRON
{
	// Particle director.
	struct Director
	{
		double x;
		double y;
		double z;

		Director(){}

		Director(double nx, double ny, double nz) : x(nx), y(ny), z(nz){} 

		Director(std::vector<double>& v)
		{
			x = v[0];
			y = v[1]; 
			z = v[2];
		}

		inline bool operator==(const Director& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
		}

		inline bool operator!=(const Director& rhs) const
		{
			return this->x != rhs.x || this->y != rhs.y || this->z != rhs.z;
		}

		inline Director& operator=(const Director& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		inline Director operator*(const double& rhs) const
		{
			Director lhs;
			lhs.x = x*rhs;
			lhs.y = y*rhs;
			lhs.z = z*rhs;

			return lhs;
		}

		inline operator std::vector<double>()
		{
			return {x, y, z};
		}

		// Get the norm of the director.
		inline double norm()
		{
			return sqrt(x*x + y*y + z*z);
		}

		// Get the norm sqaured of the director.
		inline double norm2()
		{
			return x*x + y*y + z*z;
		}

		inline double dot(const Director& rhs) const
		{
			return x*rhs.x + y*rhs.y + z*rhs.z;
		}

		// Normalize the director.
		void normalize()
		{
			double n = norm();
			x /= n;
			y /= n;
			z /= n;
		}

	};

	inline Director operator+(Director lhs, const Director& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		return lhs;
	} 


	inline Director operator-(Director lhs, const Director& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;
		return lhs;
	} 

	inline Director operator*(double lhs, const Director& rhs)
	{
		return rhs*lhs;
	}
}