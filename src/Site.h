#pragma once

#include "Visitors/Visitable.h"
#include <array>
#include <vector>

class Site : public Visitors::Visitable
{
	private:
		// Site coordinates.
		double x, y, z;

		// Site unit vectors.
		std::vector<double> u = { std::vector<double>{0.0, 0.0, 0.0} };

		// Site species
		int species = 1;

		// Neighboring sites. In a vector/array of sites, this holds the indices
		// of the sites that are considered neighbors.
		std::vector<int> neighbors;

	public:
		// Initializes a site at given coordinates.
		Site(double x, double y, double z) : x(x), y(y), z(z) {};

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
			return u[0];
		}

		// Sets the x unit vector.
		double SetXUnitVector(double newux)
		{
			return u[0] = newux;
		}

		// Get y unit vector.
		double GetYUnitVector()
		{
			return u[1];
		}

		// Sets the y unit vector.
		double SetYUnitVector(double newuy)
		{
			return u[1] = newuy;
		}

		// Get z unit vector.
		double GetZUnitVector()
		{
			return u[2];
		}

		// Sets the z unit vector.
		double SetZUnitVector(double newuz)
		{
			return u[2] = newuz;
		}

		// Gets the unit vectors
		std::vector<double>& GetUnitVectors()
		{
			return u;
		}

		// Sets all unit vectors simultaneously.
		void SetUnitVectors(double newux, double newuy, double newuz)
		{
			u[0] = newux;
			u[1] = newuy;
			u[2] = newuz;
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
		std::vector<int>& GetNeighbors()
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

		// Accept visitor.
		virtual void AcceptVisitor(class Visitors::Visitor &v)
		{
			v.Visit(this);
		}
};
