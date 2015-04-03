#pragma once

#include "Neighbor.h"
#include "../Visitors/Visitable.h"
#include "../Connectivities/Connectivity.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace SAPHRON
{
	struct Position
	{
		double x;
		double y;
		double z;

		bool operator==(const Position& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
		}
	};

	typedef std::vector<double> Director;
	typedef std::list<Neighbor> NeighborList;
	typedef std::list<Neighbor>::iterator NeighborIterator;
	typedef std::vector<std::string> SpeciesList;
	typedef std::list<Connectivity*> ConnectivityList;
	typedef std::list<Connectivity*>::iterator ConnectivityIterator;
	typedef std::vector<Particle*> ParticleList;

	// Abstract class Particle represents either a composite or primitive object, from an atom/site to
	// a molecule to a collection of molecules. It represents an common interface allowing the manipulation
	// of all of the above through a common interface.
	class Particle : public Visitable
	{
		private:

			// String species.
			std::string _species;

			// Integer species.
			int _speciesID;

			// Neighbor list.
			NeighborList _neighbors;

			// Global identifier.
			int _globalID;

			// Next ID counter for unique global species.
			static int _nextID;

			// Global list of particle identities.
			static SpeciesList _speciesList;

			// Connectivities.
			ConnectivityList _connectivities;

		public:

			// Initialize a particle with a particular species. This string represents the global type
			// species for this particle.
			Particle(std::string species) : _species(species), _speciesID(0), _globalID(++_nextID)
			{
				SetSpecies(species);
			}

			// Copy constructor.
			Particle(const Particle& particle) : 
			_species(particle._species), _speciesID(particle._speciesID), _neighbors(particle._neighbors), 
			_globalID(++_nextID), _connectivities(0)
			{
			}

			virtual ~Particle() {}

			// Get global particle species.
			int GetGlobalIdentifier() const
			{
				return _globalID;
			}

			// Get particle species.
			int GetSpeciesID() const
			{
				return _speciesID;
			}

			// Get particle string species.
			std::string GetSpecies() const
			{
				return _species;
			}

			// Get identity list.
			static SpeciesList GetSpeciesList()
			{
				return _speciesList;
			}

			// Set the species of a particle.
			void SetSpecies(std::string species);

			// Set the species of a particle.
			void SetSpecies(int id);

			// Get particle position.
			virtual Position GetPosition() const = 0;

			// Move a particle to a new set of coordinates.
			virtual void SetPosition(const Position& position) = 0;

			// Move a particle to a new set of coordinates.
			virtual void SetPosition(Position && position) = 0;

			// Get the particle director.
			virtual Director GetDirector() const = 0;

			// Get the particle director pointer.
			virtual const Director& GetDirectorRef() const = 0;

			// Set the particle director.
			virtual void SetDirector(const Director& director) = 0;

			// Set the particle director.
			virtual void SetDirector(Director && director) = 0;

			// Set the particle director.
			virtual void SetDirector(double ux, double uy, double uz) = 0;

			// Gets neighbor list iterator.
			NeighborList& GetNeighbors()
			{
				return _neighbors;
			}

			// Add a neighbor to neighbor list.
			void AddNeighbor(Neighbor && neighbor)
			{
				_neighbors.emplace_back(neighbor);
			}

			// Add a connectivity to the particle.
			void AddConnectivity(Connectivity* connectivity)
			{
				_connectivities.push_back(connectivity);
			}

			// Gets connectivity list.
			const ConnectivityList& GetConnectivities() const
			{
				return _connectivities;
			}

			virtual void AcceptVisitor(Visitor &v) override
			{
				v.Visit(this);
			}

			// Gets all descendants of a particle.
			virtual std::vector<Particle*> GetChildren() = 0;

			// Clone particle.
			virtual Particle* Clone() const = 0;
	};
}
