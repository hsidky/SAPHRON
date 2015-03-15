#pragma once

#include "Neighbor.h"
#include <list>
#include <string>
#include <memory>
#include <vector>

namespace SAPHRON
{
	struct Position
	{
		double x;
		double y;
		double z;
	};

	typedef std::vector<double> Director;
	typedef std::list<Neighbor> NeighborList;
	typedef std::list<Neighbor>::iterator NeighborIterator;
	typedef std::shared_ptr<Particle> ParticlePtr;

	// Abstract class Particle represents either a composite or primitive object, from an atom/site to
	// a molecule to a collection of molecules. It represents an common interface allowing the manipulation
	// of all of the above through a common interface.
	class Particle
	{
		private:
			std::string _identifier;
			NeighborList _neighbors;

		public:

			// Initialize a particle with a particular identifier. This string represents the global type
			// identifier for this particle.
			Particle(std::string identifier) : _identifier(identifier){}

			virtual ~Particle() {}

			std::string GetIdentifier()
			{
				return _identifier;
			}

			// Get particle position.
			virtual Position GetPosition() const = 0;

			// Move a particle to a new set of coordinates.
			virtual void SetPosition(const Position& position) = 0;

			// Get the particle director.
			virtual Director GetDirector() const = 0;

			// Set the particle director.
			virtual void SetDirector(const Director& director) = 0;

			// Gets neighbor list iterator.
			NeighborList& GetNeighborList()
			{
				return _neighbors;
			}

			// Add a neighbor to neighbor list.
			void AddNeighbor(Neighbor && neighbor)
			{
				_neighbors.emplace_back(neighbor);
			}

			// Gets all descendants of a particle.
			virtual std::vector<Particle*> GetChildren() = 0;

			// Clone particle.
			virtual Particle* Clone() const = 0;
	};
}
