#pragma once 
#include <math.h>
#include "../Properties/Vector3D.h"

// This a collection of random helper functions.
namespace SAPHRON
{
	// Create a rotation matrix which will rotate a vector 
	// abount an axis (x = 1, y = 2, z = 3) "deg" degrees.
	inline Matrix3D GenRotationMatrix(int axis, double deg)
	{
		double phi = 0, theta = 0, psi = 0;

		// assign correct angle.
		switch(axis)
		{
			case 1:	phi = deg;
				break;
			case 2:	theta = deg;
				break;
			case 3:	psi = deg;
				break;
		}

		// Compote trig functions once.
		auto cphi = cos(phi);
		auto sphi = sin(phi);
		auto ctheta = cos(theta);
		auto stheta = sin(theta);
		auto cpsi = cos(psi);
		auto spsi = sin(psi);

		// Build rotation matrix. 
		return {{ ctheta*cpsi, cphi*spsi+sphi*stheta*cpsi, sphi*spsi-cphi*stheta*cpsi},
				{-ctheta*spsi, cphi*cpsi-sphi*stheta*spsi, sphi*cpsi+cphi*stheta*spsi},
				{      stheta,               -sphi*ctheta,                cphi*ctheta}};
	}
}