#pragma once

#include "ParticleObserver.h"
#include "ParticleEvent.h"
#include "vecmap.h"
#include "json/json.h"
#include "../Observers/Visitable.h"
#include "../Connectivities/Connectivity.h"
#include "../Properties/Vector3D.h"
#include "../JSON/Serializable.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace SAPHRON
{
	inline int ffloor(double x)
	{
	    return (int) x - (x<0); 
	}

	inline int fceil(double x)
	{
	    return (int) x + (x>0);
	}

	// Minimum image convention.
	inline double anint(double x)
	{
		return ( x >= 0 ) ? ffloor( x + 0.5 ) : fceil( x - 0.5 );
	}

	typedef std::vector<Particle*> NeighborList;
	typedef std::vector<std::string> SpeciesList;
	typedef std::vector<Connectivity*> ConnectivityList;
	typedef std::vector<Particle*> ParticleList;
	typedef vecmap<int, Particle*> ParticleMap; 

	// Forward declare.
	class World;

	// Particle represents either a composite or primitive object, 
	// from an atom/site to a molecule to a collection of molecules. 
	// It represents an common interface allowing the manipulation
	// of all of the above through a common interface.
	class Particle : public Visitable, public Serializable
	{
	private:
		Position _position;
		Director _director;
		Position _checkpoint;

		double _charge;
		double _mass;

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
		std::vector<ParticleObserver*> _observers;

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

	public:
		typedef ParticleList::iterator iterator;
		typedef ParticleList::const_iterator const_iterator;

		// Initialize a particle with a particular species. This string 
		// represents the global type species for this particle.
		Particle(const Position& pos, const Director& dir, std::string species) : 
		_position(pos), _director(dir), _checkpoint(), _charge(0), _mass(1.0), _species(species), 
		_speciesID(0), _neighbors(0), _bondedneighbors(0), 
		_children(0), _observers(), _globalID(-1), _world(nullptr), _parent(nullptr),
		_connectivities(0), _pEvent(this)
		{
			_globalID = SetGlobalIdentifier(GetNextGlobalID());
			SetSpecies(species);
			_neighbors.reserve(100);
			_bondedneighbors.reserve(10);
			_observers.reserve(10);
		}

		// Initialize a particle with a particular species. This string 
		// represents the global type species for this particle.
		Particle(std::string species) : 
		_position(), _director(), _checkpoint(), _charge(0), _mass(1.0), _species(species), 
		_speciesID(0), _neighbors(0), _bondedneighbors(0), 
		_children(0), _observers(), _globalID(-1), _world(nullptr), _parent(nullptr),
		_connectivities(0), _pEvent(this)
		{
			_globalID = SetGlobalIdentifier(GetNextGlobalID());
			SetSpecies(species);
			_neighbors.reserve(100);
			_bondedneighbors.reserve(10);
			_observers.reserve(10);
		}

		// Copy constructor.
		Particle(const Particle& particle) : 
		_position(particle._position), _director(particle._director),
		_checkpoint(particle._checkpoint), _charge(particle._charge), 
		_mass(particle._mass), _species(particle._species), _speciesID(particle._speciesID), 
		_neighbors(particle._neighbors), _bondedneighbors(0), _children(0), 
		_observers(particle._observers), _globalID(-1),	_world(particle._world), 
		_parent(particle._parent), _connectivities(particle._connectivities), 
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

		/****************************
		 *                          *
		 *   Getters and Setters    *
		 *                          *
		 ****************************/

		// Get global particle ID.
		inline int GetGlobalIdentifier() const
		{
			return _globalID;
		}

		// Set global particle ID. If ID is taken, 
		// returns an alternative ID that is unique.
		inline int SetGlobalIdentifier(int id)
		{
			// ID MUST be unique. But make sure we are not 
			// re-assigning the same value.
			auto it = _identityList.find(id);
			if(it != _identityList.end() && it->second != this)
				id = GetNextGlobalID();

			// Clear previous ID and update new one.
			_identityList.erase(_globalID);
			_identityList[id] = this;
			_globalID = id;
			return id;
		}

		// Get particle parent.
		Particle* GetParent() { return _parent;	}

		Particle const* GetParent() const { return _parent; }

		// Set particle parent.
		void SetParent(Particle* particle) { _parent = particle; }

		// Gets all children of a particle.
		ParticleList& GetChildren()	{ return _children;	}

		// Gets all children of a particle.
		const ParticleList& GetChildren() const	{ return _children; }

		// Gets neighbor list iterator.
		inline NeighborList& GetNeighbors()	{ return _neighbors; }

		// Gets neighbor list iterator.
		inline const NeighborList& GetNeighbors() const	{ return _neighbors; }

		// Get the associated world.
		inline World* GetWorld() const { return _world;	}

		// Set associated world. This should not be used except by the world itself!
		void SetWorld(World* world) 
		{ 
			_world = world; 
			for(auto& child : *this)
				child->SetWorld(world);
		}

		// Get particle species.
		inline int GetSpeciesID() const	{ return _speciesID; }

		// Get particle string species.
		inline std::string GetSpecies() const {	return _species; }

		// Set the species of a particle.
		void SetSpecies(std::string species);

		// Set the species of a particle.
		void SetSpeciesID(int id);

		// Get particle charge
		double GetCharge() const
		{
			return _charge;
		}

		// Set particle charge
		void SetCharge(double charge)
		{
			if(_children.size() != 0)
			{
				std::cerr << "ERROR: Cannot set the charge for a molecule directly." << std::endl;
				exit(-1);
			}

			this->_pEvent.SetOldCharge(_charge);
			_charge = charge;
			this->_pEvent.charge = 1;
			this->NotifyObservers();

			// If we change charge of a child particle. Notify parent. 
			if(HasParent())
			{
				_parent->UpdateCharge();
				_parent->NotifyObservers();
			}
		}

		// Get particle position.
		const Position& GetPosition() const { return _position; }

		// Move a particle to a new set of coordinates.
		void SetPosition(const Position& position)
		{
			this->_pEvent.SetOldPosition(_position);
			if(_children.size() != 0)
			{
				Position dij = position - _position;
				for(auto& child : *this)
					child->SetPosition(child->GetPosition() + dij);
			}
			_position = position;
			this->_pEvent.position = 1;
			this->NotifyObservers();	
		}

		// Sets the molecule position.
		void SetPosition(double x, double y, double z)
		{
			this->_pEvent.SetOldPosition(_position);
			if(_children.size() != 0)
			{
				Position dij {_position[0] - x, _position[1] - y, _position[2] - z};
				for(auto& child : *this)
					child->SetPosition(child->GetPosition() + dij);
			}
			_position[0] = x;
			_position[1] = y;
			_position[2] = z;
			this->_pEvent.position = 1;
			this->NotifyObservers();
		}

		// Gets a particle's position at the checkpoint.
		const Position& GetCheckpoint() const
		{
			return _checkpoint;
		}

		// Get a particle's distance from checkpoint. 
		Position GetCheckpointDist() const
		{
			return _position - _checkpoint;
		}

		// Sets a checkpoint where the particle records its position at that moment. 
		void SetCheckpoint()
		{
			_checkpoint = _position;
			
			for(auto& c : *this)
				c->SetCheckpoint();	
		}

		// Get the particle director.
		const Director& GetDirector() const
		{
			return _director;
		}

		// Set the particle director.
		void SetDirector(const Director& director)
		{
			this->_pEvent.SetOldDirector(_director);
			_director = director;
			this->_pEvent.director = 1;
			this->NotifyObservers();
		}

		// Set the particle director.
		void SetDirector(double ux, double uy, double uz)
		{
			this->_pEvent.SetOldDirector(_director);
			_director[0] = ux;
			_director[1] = uy;
			_director[2] = uz;
			this->_pEvent.director = 1;
			this->NotifyObservers();
		}
		
		// Get the mass of a particle.
		double GetMass() const 
		{
			return _mass;
		}

		void SetMass(double mass)
		{
			if(_children.size() != 0)
			{
				std::cerr << "ERROR: Cannot set the mass for a molecule directly." << std::endl;
				exit(-1);
			}

			_mass = mass;
		}

		/****************************
		 *                          *
		 *      Basic helpers       *
		 *                          *
		 ****************************/

		// Update center of (and) mass (containing children).
		void UpdateCenterOfMass()
		{
			if(_children.size() != 0)
			{
				// We don't fire event here because it's fired by the 
				// caller.
				this->_pEvent.SetOldPosition(_position);
				this->_pEvent.position = 1;

				_position = {0, 0, 0};
				_mass = 0; 
				for(auto& child : *this)
				{
					_position += child->GetPosition()*child->GetMass();
					_mass += child->GetMass();
				}	

				// Avoid divide by zero.
				if(_mass) _position /= _mass;
			}
		}

		// Updates the charge for a particle (containing children).
		void UpdateCharge()
		{
			if(_children.size() != 0)
			{

				this->_pEvent.SetOldCharge(_charge);
				this->_pEvent.charge = 1;

				_charge = 0;
				for(auto& child : *this)
					_charge += child->GetCharge();
			}
		}

		// Has Parent?
		bool HasParent() const { return _parent != nullptr; }

		// Clear parent particle.
		void ClearParent() { _parent = nullptr; }

		// Check if particle has children.
		bool HasChildren() const { return _children.size() != 0; }

		// Get index of particle in parent child list.
		// Returns -1 if no parent.
		int GetChildIndex() const
		{
			if(!HasParent()) return -1;

			auto it = std::find(_parent->begin(), _parent->end(), this);
			return it - _parent->begin();
		}

		/****************************
		 *                          *
		 *  Connectivity related    *
		 *                          *
		 ****************************/
		
		// Add a child. Children MUST be heap allocated! 
		// Any child belonging to a particle at time of 
		// destruction will be freed.
		void AddChild(Particle* child);
		
		// Remove a child 
		void RemoveChild(Particle* particle);

		// Removes self from neighbors.
		// Propogates to children.
		void RemoveFromNeighbors();

		// Removes self from bondedneighbors list.
		// Propogates to children.
		void RemoveFromBondedNeighbors();

		// Add a neighbor to neighbor list.
		// TODO: figure out an efficient mechanism to check for duplicates 
		// other than std::find.
		inline void AddNeighbor(Particle* particle)
		{
			//auto found = std::find(_neighbors.begin(), _neighbors.end(), particle);
			//if(found == _neighbors.end())
			_neighbors.push_back(particle);
		}

		// Remove a neighbor from the neighbor list.
		inline void RemoveNeighbor(Particle* particle)
		{
			auto it = std::find(_neighbors.begin(), _neighbors.end(), particle);
			if(it != _neighbors.end())
			{
				std::swap(*it, _neighbors.back());
				_neighbors.pop_back();
			}
		}

		// Check if a particle is a neighbor.
		inline bool IsNeighbor(const Particle& particle) const
		{
			auto found = std::find(_neighbors.begin(), _neighbors.end(), &particle);
			return  found != _neighbors.end();
		}

		// Clear the neighbor list. 
		// Propogates to children.
		inline void ClearNeighborList() 
		{ 
			_neighbors.clear(); 
			
			for(auto& c : *this)
				c->ClearNeighborList();
		}

		// Gets neighbor list iterator.
		inline NeighborList& GetBondedNeighbors() 
		{ 
			return _bondedneighbors; 
		}

		// Gets neighbor list iterator.
		inline const NeighborList& GetBondedNeighbors() const 
		{	
			return _bondedneighbors; 
		}

		// Add a neighbor to bonded neighbor list.
		inline void AddBondedNeighbor(Particle* particle)
		{
			auto found = std::find(_bondedneighbors.begin(), _bondedneighbors.end(), particle);
			if(found == _bondedneighbors.end())
				_bondedneighbors.push_back(particle);
		}

		// Remove a bonded neighbor from the bonded neighbor list.
		inline void RemoveBondedNeighbor(Particle* particle)
		{
			_bondedneighbors.erase(
				std::remove(_bondedneighbors.begin(), _bondedneighbors.end(), particle), 
				_bondedneighbors.end()
			);
		}

		// Clear the bonded neighbor list.
		inline void ClearBondedNeighborList() { _bondedneighbors.clear(); }

		// Check if a particle is a bonded neighbor.
		inline bool IsBondedNeighbor(Particle* particle) const
		{
			auto found = std::find(_bondedneighbors.begin(), _bondedneighbors.end(), particle);
			return  found != _bondedneighbors.end();
		}

		// Add a connectivity to the particle.
		void AddConnectivity(Connectivity* connectivity)
		{
			_connectivities.push_back(connectivity);
		}

		// Remove a connectivity from the particle.
		void RemoveConnectivity(Connectivity* connectivity)
		{
			_connectivities.erase(
				std::remove(_connectivities.begin(), _connectivities.end(), connectivity),
				_connectivities.end());
		}

		// Gets connectivity list.
		const ConnectivityList& GetConnectivities() const
		{
			return _connectivities;
		}

		/****************************
		 *                          *
		 *     Other interfaces     *
		 *                          *
		 ****************************/

		// Add particle observer. Propogates to children.
		inline void AddObserver(ParticleObserver* observer)
		{
			_observers.push_back(observer);
			for(auto& c : _children)
				c->AddObserver(observer);
		}

		// Remove particle observer. Propogates to children.
		inline void RemoveObserver(ParticleObserver* observer)
		{
			_observers.erase(
				std::remove(_observers.begin(), _observers.end(),observer), 
				_observers.end()
			);
			for(auto& c : _children)
				c->RemoveObserver(observer);
		}

		// Notify registered particle observers of a change.
		inline void NotifyObservers()
		{
			for (auto observer : _observers)
				observer->ParticleUpdate(_pEvent);

			_pEvent.mask = 0;
		}

		// Get the number of observers.
		inline int ObserverCount() const {	return (int)_observers.size(); }

		void AcceptVisitor(Visitor& v) const override
		{
			for (auto &c :_children)
				c->AcceptVisitor(v);
			v.Visit(*this);
		}

		// Clone particle.
		Particle* Clone() const
		{
			return new Particle(*this);
		}

		/****************************
		 *                          *
		 * Building and static fxns *
		 *                          *
		 ****************************/

		// Get particle blueprint.
		void GetBlueprint(Json::Value& json) const;

		// Serialize particle.
		void Serialize(Json::Value& json) const override
		{
			json[0] = GetGlobalIdentifier();
			json[1] = GetSpecies();

			auto& pos = GetPosition();
			json[2][0] = pos[0];
			json[2][1] = pos[1];
			json[2][2] = pos[2];

			auto& dir = GetDirector();
			json[3][0] = dir[0];
			json[3][1] = dir[1];
			json[3][2] = dir[2];
		}


		// Get reference to global particle map.
		static const ParticleMap& GetParticleMap() { return _identityList; }

		// Get species ID from string.
		static int GetSpeciesID(std::string id) 
		{
			auto p1 = std::find(_speciesList.begin(), _speciesList.end(), id);
			return p1 - _speciesList.begin();
		}

		// Get species list.
		static const SpeciesList& GetSpeciesList() { return _speciesList;	}

		// Build a particle. This builds a particle from JSON array and the blueprint. 
		// If the particle is a composite object, an commensurate number of particles 
		// in a JSON array must be passed in.
		static Particle* BuildParticle(const Json::Value& particles, 
									   const Json::Value& blueprint);

		// Build particles. This builds particles from a JSON array of primitive
		// particles, molecule blueprints, and component counts. It then stores the 
		// created particles in pvector. Object lifetime is the responsibility of 
		// the caller! If an exception is thrown the caller must clean up instantiated 
		// object in pvector.
		static void BuildParticles(const Json::Value& particles, 
								   const Json::Value& blueprints,
								   const Json::Value& components,
								   ParticleList& pvector);

		// Iterators.
		iterator begin() { return _children.begin(); }
		const_iterator begin() const { return _children.begin(); }
		iterator end() { return _children.end(); }
		const_iterator end() const { return _children.end(); }
	};
}
