#pragma once 

#include "World.h"
#include "../Utils/Rand.h"
#include "../Observers/Visitable.h"

namespace SAPHRON
{
	// WorldManager class that manages worlds for simulations. 
	class WorldManager : public Visitable
	{
	private:
		// List of worlds. 
		WorldList _worlds;

		// Active world.
		World* _active;

		// Random number generator.
		Rand _rand;

		// Helper method to remove items from vectors.
		template <typename T> 
		void remove(std::vector<T>& vec, size_t pos) 
		{ 
			vec.erase(vec.begin() + pos); 
		}

	public:
		typedef WorldList::iterator iterator;
		typedef WorldList::const_iterator const_iterator;

		WorldManager(int seed = 1090) : 
		_worlds(0), _active(nullptr), _rand(seed) {}

		// Adds a world to the world list. Note: Last added world becomes 
		// the "active" world.
		void AddWorld(World* world)
		{
			if(std::find(_worlds.begin(), _worlds.end(), world) == _worlds.end())
			{
				_worlds.push_back(world);
				_active = world;
			}
		}

		// Removes a world from the world list.
		void RemoveWorld(World* world)
		{
			size_t pos = std::find(_worlds.begin(), _worlds.end(), world) - _worlds.begin();
			if(pos < _worlds.size())
			{
				remove(_worlds, pos);

				// If it was active world, remove it.
				if(_active == world)
					_active = _worlds.back();
			}
		}

		// Sets the active world to "i".
		void SetActiveWorld(size_t i)
		{
			assert(i < _worlds.size());				
			_active = _worlds[i];			
		}

		// Sets the active world to "World*". 
		// If world does not exist in the list, it is added.
		void SetActiveWorld(World* world)
		{
			if(std::find(_worlds.begin(), _worlds.end(), world) == _worlds.end())
				AddWorld(world);

			_active = world;
		}

		// Returns active world.
		World* GetWorld() { return _active; }

		// Returns active world (const).
		World* GetWorld() const {return _active; }

		// Returns world "i". Throws out of range exception for invalid index.
		World* GetWorld(size_t i)
		{
			assert(i < _worlds.size());	
			return _worlds[i];
		}

		// Returns world "i" (const).
		World* GetWorld(size_t i) const { return GetWorld(i); }

		// Returns the number of worlds.
		size_t GetWorldCount() const { return _worlds.size(); }

		// Returns a random world.
		World* GetRandomWorld()
		{
			return _worlds[_rand.int32() % _worlds.size()];
		}

		// Accept visitor.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
			for(auto& w : _worlds)
				w->AcceptVisitor(v);
		}

		// Iterators.
		iterator begin() { return _worlds.begin(); }
		iterator end() { return _worlds.end(); }
		const_iterator begin() const { return _worlds.begin(); }
		const_iterator end() const { return _worlds.end(); }
	};
}