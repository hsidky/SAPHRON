#include "../Worlds/World.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Simulation class that manages resources for all simulation related 
	// actions. It also stores simulation conditions: Temperature, pressure, energies,
	// etc...
	class Simulation
	{
	private:
		// List of worlds. 
		WorldList _worlds;

		// List of temperatures.
		std::vector<double> _temperatures;

		// Random number generator.
		Rand _rand;

		// Helper method to remove items from vectors.
		template <typename T> 
		void remove(std::vector<T>& vec, size_t pos) 
		{ 
			vec.erase(vec.begin() + pos); 
		}

	public:
		Simulation(int seed = 1090) : 
		_worlds(0), _temperatures(0), _rand(seed) {}

		// Adds a world to the world list. Note: Last added world becomes 
		// the "active" world.
		void AddWorld(World* world)
		{
			if(std::find(_worlds.begin(), _worlds.end(), world) == _worlds.end())
			{
				_worlds.push_back(world);
				_temperatures.push_back(0);
			}
		}

		// Removes a world from the world list.
		void RemoveWorld(World* world)
		{
			int pos = std::find(_worlds.begin(), _worlds.end(), world) - _worlds.begin();
			if(pos < _worlds.size())
			{
				remove(_worlds, pos);
				remove(_temperatures, pos);
			}
		}

		// Returns active world.
		World* GetWorld()
		{
			return _worlds.back();
		}

		// Returns world "i".
		World* GetWorld(int i)
		{
			assert(i < _worlds.size() - 1);
			return _worlds[i];
		}

		// Returns a random world.
		World* GetRandomWorld()
		{
			return _worlds[_rand.int32() % _worlds.size()];
		}


	};
}