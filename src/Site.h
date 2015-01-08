#pragma once

#include <array>
#include <vector>

class Site
{
	private:
		// Site coordinates.
		double x, y, z;

		// Site unit vectors.
		double ux, uy, uz;

		// Site species
		int species = 0;

		// Neighboring sites. In a vector/array of sites, this holds the indices
		// of the sites that are considered neighbors.
		std::vector<int> neighbors;

	public:
		Site() {};

		// Gets x coordinate.
		double GetXCoordinate()
		{
			return x;
		}

		// Sets the x coordinate independently.
		double SetXCoordinate(double newX)
		{
			return x = newX;
		}

		// Gets y coordinate.
		double GetYCoordinate()
		{
			return y;
		}

		// Sets the y coordinate independently.
		double SetYCoordinate(double newY)
		{
			return y = newY;
		}

		// Gets z coordinate.
		double GetZCoordinate()
		{
			return z;
		}

		// Sets the z coordinate independently.
		double SetZCoordinate(double newZ)
		{
			return z = newZ;
		}

		// Gets a vector of coordinates.
		std::vector<double> GetCoordinates()
		{
			return {x, y, z};
		}

		// Sets all coordinates simultaneously.
		void SetCoordinates(double newX, double newY, double newZ)
		{
			x = newX;
			y = newY;
			z = newZ;
		}

		// Get x unit vector.
		double GetXUnitVector()
		{
			return ux;
		}

		// Sets the x unit vector.
		double SetXUnitVector(double newux)
		{
			return ux = newux;
		}

		// Get y unit vector.
		double GetYUnitVector()
		{
			return uy;
		}

		// Sets the y unit vector.
		double SetYUnitVector(double newuy)
		{
			return uy = newuy;
		}

		// Get z unit vector.
		double GetZUnitVector()
		{
			return uz;
		}

		// Sets the z unit vector.
		double SetZUnitVector(double newuz)
		{
			return uz = newuz;
		}

		// Gets the unit vectors
		std::vector<double> GetUnitVectors()
		{
			return {ux, uy, uz};
		}

		// Sets all unit vectors simultaneously.
		void SetUnitVectors(double newux, double newuy, double newuz)
		{
			ux = newux;
			uy = newuy;
			uz = newuz;
		}

		// Gets species type.
		int GetSpecies()
		{
			return species;
		}

		// Sets species type.
		int SetSpecies(int type)
		{
			return species = type;
		}

		// Gets site neighbors.
		std::vector<int> GetNeighbors()
		{
			return neighbors;
		}

		// Adds a neighbor to the end of the existing list.
		void AppendNeighbor(int newNeighbor)
		{
			return neighbors.push_back(newNeighbor);
		}

		// Removes the last neighbor in the list.
		void RemoveLastNeighbor()
		{
			return neighbors.pop_back();
		}

		// Sets site neighbors.
		std::vector<int> SetNeighbors(std::vector<int> newNeighbors)
		{
			return neighbors = newNeighbors;
		}
};
