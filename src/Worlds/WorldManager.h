#pragma once 

#include "NewWorld.h"
#include "../Utils/Rand.h"
#include "../Observers/Visitable.h"
#include "../JSON/Serializable.h"

namespace SAPHRON
{
	// WorldManager class that manages worlds for simulations. 
	class WorldManager : public Visitable, public Serializable
	{
	private:
		// List of worlds. 
		WorldList worlds_;

		// Active world.
		NewWorld* active_;

		// Random number generator.
		Rand rand_;

	public:
		typedef WorldList::iterator iterator;
		typedef WorldList::const_iterator const_iterator;

		WorldManager(unsigned seed = 1090) : 
		worlds_(0), active_(nullptr), rand_(seed) {}

		// Adds a world to the world list. Note: Last added world becomes 
		// the "active" world.
		void AddWorld(NewWorld* world)
		{
			if(std::find(worlds_.begin(), worlds_.end(), world) == worlds_.end())
			{
				worlds_.push_back(world);
				active_ = world;
			}
		}

		// Removes a world from the world list.
		void RemoveWorld(NewWorld* world)
		{
			worlds_.erase(std::remove(worlds_.begin(), worlds_.end(), world), worlds_.end());
			if(active_ == world)
				active_ = worlds_.back();
		}

		// Sets the active world to "i".
		void SetActiveWorld(size_t i)
		{
			assert(i < worlds_.size());				
			active_ = worlds_[i];			
		}

		// Sets the active world to "NewWorld*". 
		// If world does not exist in the list, it is added.
		void SetActiveWorld(NewWorld* world)
		{
			if(std::find(worlds_.begin(), worlds_.end(), world) == worlds_.end())
				AddWorld(world);

			active_ = world;
		}

		// Returns active world.
		NewWorld* GetWorld() { return active_; }

		// Returns active world (const).
		NewWorld* GetWorld() const {return active_; }

		// Returns world "i". Throws out of range exception for invalid index.
		NewWorld* GetWorld(size_t i)
		{
			if(i >= worlds_.size())
				throw std::out_of_range("World ID is out of range.");
			return worlds_[i];
		}

		// Returns world "i" (const).
		NewWorld* GetWorld(size_t i) const { return GetWorld(i); }

		// Returns the number of worlds.
		size_t GetWorldCount() const { return worlds_.size(); }

		// Returns a random world.
		NewWorld* GetRandomWorld()
		{
			return worlds_[rand_.int32() % worlds_.size()];
		}

		// Accept visitor.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
			for(auto& w : worlds_)
				w->AcceptVisitor(v);
		}

		virtual void Serialize(Json::Value& json) const override
		{
			auto& worlds = json["worlds"];
			///for(int i = 0; i < (int)worlds_.size(); ++i)
			//	worlds_[i]->Serialize(worlds[i]);
		}

		// Iterators.
		iterator begin() { return worlds_.begin(); }
		iterator end() { return worlds_.end(); }
		const_iterator begin() const { return worlds_.begin(); }
		const_iterator end() const { return worlds_.end(); }
	};
}