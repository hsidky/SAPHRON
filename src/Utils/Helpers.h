#pragma once 
#include <cstdio>
#include <cerrno>
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

	// Retrieves the contents of a file and returns them
	// in a string. Throws exception on failure.
	inline std::string GetFileContents(const char *filename)
	{
		std::FILE *fp = std::fopen(filename, "rb");
		if (fp)
		{
			std::string contents;
			std::fseek(fp, 0, SEEK_END);
			contents.resize(std::ftell(fp));
			std::rewind(fp);

			// Stupid GCC bug. We do this to hide warnings.
			if(!std::fread(&contents[0], 1, contents.size(), fp))
				std::fclose(fp);
			else
				std::fclose(fp);

			return(contents);
		}
		throw(errno);
	}

	// Gets file path from filename.
	inline std::string GetFilePath(const std::string& str)
	{
		size_t found;
		found = str.find_last_of("/\\");
		if(found == str.npos)
			return "./";
		return str.substr(0, found);
	}

	//Compute square of a number. 
	template<typename  T>
	inline T fsq(const T x) { return x*x; }
	
	//Compute cube of a number. 
	template<typename  T>
	inline T fcb(const T x) { return x*x*x; }
	
	//Compute power 4 of a number. 
	template<typename  T>
	inline T fpow4(const T x) { return x*x*x*x; }
	
	//Compute power 5 of a number. 
	template<typename  T>
	inline T fpow5(const T x) { return x*x*x*x*x; }

	//Compute power 6 of a number. 
	template<typename  T>
	inline T fpow6(const T x) { return x*x*x*x*x*x; }
}