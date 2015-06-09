#pragma once 

#include <cmath>

namespace SAPHRON
{
	// Particle position.
	struct Position
	{
		double x;
		double y;
		double z;

		inline bool operator==(const Position& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
		}

		inline double norm() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		inline double norm2() const
		{
			return (x*x + y*y + z*z);
		}
	};

	inline Position operator+(Position lhs, const Position& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		return lhs;
	} 


	inline Position operator-(Position lhs, const Position& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;
		return lhs;
	} 
}