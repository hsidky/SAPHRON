#pragma once

#include "../Observers/Visitable.h"
#include "../Connectivities/Connectivity.h"
#include "ParticleObserver.h"
#include "ParticleEvent.h"
#include "Position.h"
#include "Director.h"
#include "json/json.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <map>

namespace SAPHRON
{
	inline int ffloor(const double& x)
	{
	    return (int) x - (x<0); 
	}

	inline int fceil(const double& x)
	{
	    return (int) x + (x>0);
	}

	// Minimum image convention.
	inline double anint(const double& x)
	{
		return ( x >= 0 ) ? ffloor( x + 0.5 ) : fceil( x - 0.5 );
	}

	typedef std::vector<Particle*> NeighborList;
	typedef std::vector<Particle*>::iterator NeighborIterator;
	typedef std::vector<std::string> SpeciesList;
	typedef std::list<Connectivity*> ConnectivityList;
	typedef std::list<Connectivity*>::iterator ConnectivityIterator;
	typedef std::vector<Particle*> ParticleList;
	typedef std::map<int, Particle*> ParticleMap;

	// Forward declare.
	class World;

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

			// Bonded List.
			NeighborList _bondedneighbors;

			// Particle children.
			ParticleList _children;

			// Observer list. 
			std::list<ParticleObserver*> _observers;

			// Global identifier.
			int _globalID;

			// Associated world.
			World* _world;

			// Parent particle.
			Particle* _parent;

			// Next ID counter for unique global species.
			static int _nextID;

			// Global list of particle species.
			static SpeciesList _speciesList;

			// Global list of particle id's and pointers.
			static ParticleMap _identityList;

			// Connectivities.
			ConnectivityList _connectivities;

			// Gets the next available global ID.
			int GetNextGlobalID()
			{
				int candidate = ++_nextID;
				if(_identityList.find(candidate) == _identityList.end())
					return candidate;
				else
					return GetNextGlobalID();
			}
		
		protected:

			// Particle event. 
			ParticleEvent _pEvent;

			// Update center of mass.
			virtual void UpdateCenterOfMass() {}

		public:
			typedef ParticleList::iterator iterator;
			typedef ParticleList::const_iterator const_iterator;

			// Initialize a particle with a particular species. This string represents the global type
			// species for this particle.
			Particle(std::string species) : 
			_species(species), _speciesID(0), _neighbors(0), _bondedneighbors(0), 
			_children(0), _observers(), _globalID(-1), _world(nullptr), _parent(nullptr),
			_connectivities(0), _pEvent(this)
			{
				_globalID = SetGlobalIdentifier(GetNextGlobalID());
				SetSpecies(species);
				_neighbors.reserve(100);
				_bondedneighbors.reserve(10);
			}

			// Copy constructor.
			Particle(const Particle& particle) : 
			_species(particle._species), _speciesID(particle._speciesID), _neighbors(particle._neighbors),
			_bondedneighbors(0), _children(0), _observers(particle._observers), _globalID(-1),
			_world(particle._world), _parent(particle._parent), _connectivities(particle._connectivities), 
			_pEvent(this)
			{
				_globalID = SetGlobalIdentifier(GetNextGlobalID());
				for(const auto& child : particle)
					this->AddChild(child->Clone());

				// Loop through children and update bonded.
				int i = 0;
				for(const auto& child : particle)
				{
					auto& bonded = child->GetBondedNeighbors();
					for(auto& b : bonded)
					{
						// Find position of bonded particle in the parent 
						// particle and use the index to connect the copy. 
						auto it = std::find(particle.begin(), particle.end(), b);
						assert(it != particle.end()); // this must be true!
						auto pos = it - particle.begin();
						_children[i]->AddBondedNeighbor(_children[pos]);
					}
					++i;
				}
			}

			virtual ~Particle() 
			{
				// Remove particle from map.
				_identityList.erase(_globalID);
				
				// Delete children.
				for(auto& c : _children)
				{
					delete c;
					c = nullptr;
				}
				_children.clear();

				RemoveFromNeighbors();
				RemoveFromBondedNeighbors();
			}

			// Removes self from neighbors.
			void RemoveFromNeighbors()
			{
				// Remove particle from neighbor list. 
				for(auto& neighbor : _neighbors)
				{
					if(neighbor != NULL && neighbor != nullptr)
						neighbor->RemoveNeighbor(this);
				}
			}

			//Removes self from bondedneighbors list
			void RemoveFromBondedNeighbors()
			{
				for(auto& neighbor : _bondedneighbors)
				{
					if(neighbor != NULL && neighbor != nullptr)
						neighbor->RemoveBondedNeighbor(this);
				}
			}

			// Get global particle ID.
			inline int GetGlobalIdentifier() const
			{
				return _globalID;
			}

			// Set global particle ID. If ID is taken, returns an alternative ID that is unique.
			inline int SetGlobalIdentifier(int id)
			{
				// ID MUST be unique. But make sure we are not re-assigning the same value.
				auto it = _identityList.find(id);
				if(it != _identityList.end() && it->second != this)
					id = GetNextGlobalID();

				// Clear previous ID and update new one.
				_identityList.erase(_globalID);
				_identityList[id] = this;
				_globalID = id;
				return id;
			}

			// Set particle parent.
			void SetParent(Particle* particle) { _parent = particle; }

			// Get particle parent.
			Particle* GetParent() { return _parent;	}

			Particle const* GetParent() const { return _parent; }

			// Has Parent?
			bool HasParent() const { return _parent != nullptr; }

			// Clear parent particle.
			void ClearParent() { _parent = nullptr; }

			// Set associated world. This should not be used except by the world itself!
			void SetWorld(World* world) 
			{ 
				_world = world; 
				for(auto& child : *this)
					child->SetWorld(world);
			}

			// Get the associated world.
			inline World* GetWorld() const { return _world;	}

			// Get reference to global particle map.
			static const ParticleMap& GetParticleMap() { return _identityList; }

			// Get species ID from string.
			static int GetSpeciesID(std::string id) 
			{
				auto p1 = std::find(_speciesList.begin(), _speciesList.end(), id);
				return p1 - _speciesList.begin();
			}

			// Get particle species.
			inline int GetSpeciesID() const	{ return _speciesID; }

			// Get particle string species.
			inline std::string GetSpecies() const {	return _species; }

			// Get species list.
			static SpeciesList& GetSpeciesList() { return _speciesList;	}

			// Set the species of a particle.
			void SetSpecies(std::string species);

			// Set the species of a particle.
			void SetSpecies(int id);

			// Get particle charge
			virtual double GetCharge() const = 0;

			// Set particle charge
			virtual void SetCharge(double charge) = 0;

			// Get particle position.
			virtual Position GetPosition() const = 0;

			// Get particle position reference.
			virtual const Position& GetPositionRef() const = 0;

			// Move a particle to a new set of coordinates.
			virtual void SetPosition(const Position& position) = 0;

			// Move a particle to a new set of coordinates.
			virtual void SetPosition(Position && position) = 0;

			// Set a particle position.
			virtual void SetPosition(double x, double y, double z) = 0;

			// Sets a checkpoint where the particle records its position at that moment. 
			virtual void SetCheckpoint() = 0;

			// Gets a particle's position at the checkpoint.
			virtual const Position& GetCheckpoint() const = 0;

			// Get distance from checkpoint.
			virtual Position GetCheckpointDist() const = 0;

			// Get the particle director.
			virtual Director GetDirector() const = 0;

			// Get the particle director pointer.
			virtual const Director& GetDirectorRef() const = 0;

			// Set the particle director.
			virtual void SetDirector(const Director& director) = 0;

			// Set the particle director.
			virtual void SetDirector(Director&& director) = 0;

			// Set the particle director.
			virtual void SetDirector(double ux, double uy, double uz) = 0;

			// Get the mass of a particle.
			virtual double GetMass() = 0;

			// Gets neighbor list iterator.
			inline NeighborList& GetNeighbors()	{ return _neighbors; }

			// Gets neighbor list iterator.
			inline const NeighborList& GetNeighbors() const	{ return _neighbors; }

			// Add a neighbor to neighbor list.
			// TODO: figure out an efficient mechanism to check for duplicates other than std::find.
			inline void AddNeighbor(Particle* particle)
			{
				if(std::find(_neighbors.begin(), _neighbors.end(), particle) == _neighbors.end())
					_neighbors.push_back(particle);
			}

			// Remove a neighbor from the neighbor list.
			inline void RemoveNeighbor(Particle* particle)
			{
				_neighbors.erase(std::remove(_neighbors.begin(), _neighbors.end(), particle), _neighbors.end());
			}

			// Check if a particle is a neighbor.
			inline bool IsNeighbor(Particle* particle) const
			{
				return std::find(_neighbors.begin(), _neighbors.end(), particle) != _neighbors.end();
			}

			// Clear the neighbor list.
			inline void ClearNeighborList() { _neighbors.clear(); }

			// Gets neighbor list iterator.
			inline NeighborList& GetBondedNeighbors() {	return _bondedneighbors; }

			// Gets neighbor list iterator.
			inline const NeighborList& GetBondedNeighbors() const {	return _bondedneighbors; }

			// Add a neighbor to bonded neighbor list.
			// TODO: figure out an efficient mechanism to check for duplicates other than std::find.
			inline void AddBondedNeighbor(Particle* particle)
			{
				//if(std::find(_neighbors.begin(), _neighbors.end(), particle) == _neighbors.end())
					_bondedneighbors.push_back(particle);
			}

			// Remove a bonded neighbor from the bonded neighbor list.
			inline void RemoveBondedNeighbor(Particle* particle)
			{
				_bondedneighbors.erase(std::remove(_bondedneighbors.begin(), _bondedneighbors.end(), particle), _bondedneighbors.end());
			}

			// Clear the bonded neighbor list.
			inline void ClearBondedNeighborList() { _bondedneighbors.clear(); }

			// Check if a particle is a bonded neighbor.
			inline bool IsBondedNeighbor(Particle* particle) const
			{
				return std::find(_bondedneighbors.begin(), _bondedneighbors.end(), particle) != _bondedneighbors.end();
			}

			// Add a connectivity to the particle.
			void AddConnectivity(Connectivity* connectivity)
			{
				_connectivities.push_back(connectivity);
			}

			// Remove a connectivity from the particle.
			void RemoveConnectivity(Connectivity* connectivity)
			{
				_connectivities.remove(connectivity);
			}

			// Gets connectivity list.
			const ConnectivityList& GetConnectivities() const
			{
				return _connectivities;
			}

			// Add particle observer.
			inline void AddObserver(ParticleObserver* observer)
			{
				_observers.push_back(observer);
				for(auto& c : _children)
					c->AddObserver(observer);
			}

			// Remove particle observer.
			inline void RemoveObserver(ParticleObserver* observer)
			{
				_observers.remove(observer);
				for(auto& c : _children)
					c->RemoveObserver(observer);
			}

			// Notify registered particle observers of a change.
			inline void NotifyObservers()
			{
				for (auto observer : _observers)
					observer->ParticleUpdate(_pEvent);

				_pEvent.mask = false;
			}

			// Get the number of observers.
			inline int ObserverCount()
			{	
				return (int)_observers.size();
			}

			virtual void AcceptVisitor(Visitor &v) override
			{
				v.Visit(*this);
			}

			// Check if particle has children.
			bool HasChildren() const { return _children.size() != 0; }

			// Add a child. Children MUST be heap allocated! 
			// Any child belonging to a particle at time of 
			// destruction will be freed.
			void AddChild(Particle* child) 
			{
				child->SetParent(this);
				child->SetWorld(_world);
				_children.push_back(child);
				UpdateCenterOfMass();
				for(auto& o : _observers)
					child->AddObserver(o);
			}

			// Remove a child 
			void RemoveChild(Particle* particle)
			{
				particle->ClearParent();
				particle->SetWorld(nullptr);
				_children.erase(std::remove(_children.begin(), _children.end(), particle), _children.end());
				UpdateCenterOfMass();
				for(auto& o : _observers)
					particle->RemoveObserver(o);
			}

			// Gets all descendants of a particle.
			ParticleList& GetChildren()
			{
				return _children;
			}

			// Gets all descendents of a particle.
			const ParticleList& GetChildren() const
			{
				return _children;
			}

			// Clone particle.
			virtual Particle* Clone() const = 0;

			// Build a particle. This builds a particle from JSON array and the blueprint. 
			// If the particle is a composite object, an commensurate number of particles 
			// in a JSON array must be passed in.
			static Particle* BuildParticle(const Json::Value& particles, const Json::Value& blueprint);

			// Build particles. This builds particles from JSON array and the blueprint. 
			// It then stores the created particles in pvector. Object lifetime is the 
			// responsibility of the caller! If an exception is thrown the caller must 
			// clean up instantiated object in pvector.
			static void BuildParticles(const Json::Value& particles, 
									   const Json::Value& blueprints, 
									   ParticleList& pvector);

			// Iterators.
			iterator begin() { return _children.begin(); }
			const_iterator begin() const { return _children.begin(); }
			iterator end() { return _children.end(); }
			const_iterator end() const { return _children.end(); }
	};
}
