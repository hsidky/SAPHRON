#pragma once 

#include "NewParticle.h"
#include <memory>

namespace SAPHRON
{
	// Manages particle blueprints and resolves species ids (uint) to strings. 
	class BlueprintManager
	{
	private:
		std::vector<Site> sites_;
		std::vector<std::unique_ptr<NewParticle>> particles_;
		std::vector<std::string> species_;
	
		// Private copy constructor for singleton. 
		BlueprintManager(const BlueprintManager&);

	public:
		BlueprintManager() : sites_(0), particles_(0), species_(0)
		{}

		// Get singleton instance of BlueprintManager.
		static BlueprintManager& Instance()
		{
			static BlueprintManager instance;
			return instance;
		}

		// Add a blueprint to the blueprint manager and associates with species string. 
		// The species id inside the particle will be used to index the string. Will 
		// throw an error if a previous blueprint is registered with a species string.
		void AddBlueprint(const std::string& species, const NewParticle& p)
		{
			auto id = p.GetSpecies();
			if(id >= particles_.size())
				particles_.resize(id+1);

			if(particles_[id])
			{
				std::cerr << "ERROR: Species \"" << species << "\" is already registered "
				"with the blueprint manager. Please remove the previous blueprint before "
				"registering." << std::endl;
				exit(-1);
			}
			particles_[id] = std::unique_ptr<NewParticle>(new NewParticle(p, sites_));
			particles_[id]->SetPosition({0, 0,0 }); // Center it so coordinates are local frame.
			AddSpeciesIndex(species, id);
		}

		// Removes a blueprint from the blueprint manager.
		void RemoveBlueprint(const std::string& species)
		{
			auto id = GetSpeciesIndex(species);
			particles_[id].reset();
			RemoveSpeciesIndex(species);
		}

		// Gets a copy of a particle from the blueprint storage. Vector must be 
		// passed in to be filled with sites.
		NewParticle GetBlueprint(const std::string& species, std::vector<Site>& sites) const
		{
			auto id = GetSpeciesIndex(species);
			return {*particles_[id], sites};
		}

		// Gets a copy of a particle from the blueprint storage. Vector must be 
		// passed in to be filled with sites.
		NewParticle GetBlueprint(uint species, std::vector<Site>& sites) const
		{
			return {*particles_[species], sites};
		}

		// Add a species index to the species/index map. 
		void AddSpeciesIndex(const std::string& species, uint id)
		{
			if(id >= species_.size())
				species_.resize(id+1);
			species_[id] = species;
		}

		// Removes a species index from the species map.
		void RemoveSpeciesIndex(const std::string& species)
		{
			auto id = GetSpeciesIndex(species);
			species_[id].clear();
		}

		// Gets the index associated with a species string. Returns vector size on failure.
		uint GetSpeciesIndex(const std::string& species) const
		{
			return std::find(species_.begin(), species_.end(), species) - species_.begin();
		}

		// Gets the size of the blueprint vector. This is not necessarily the 
		// number of registered blueprints. 
		uint GetBlueprintSize() const
		{
			return particles_.size();
		}

		// Gets the number of registered blueprints.
		uint GetBlueprintCount() const
		{
			return std::count_if(particles_.begin(), particles_.end(), 
				[](const std::unique_ptr<NewParticle>& p)
				{
					return !(!p);
				}
			);
		}

		// Get string associated with species ID.
		std::string GetSpeciesString(uint id) const
		{
			return species_[id];
		}

		// Checks if a species string is registered with the manager.
		bool IsRegistered(const std::string& species) const
		{
			return std::find(species_.begin(), species_.end(), species) != species_.end();
		}

		// Gets the next available (unused) id.
		uint GetNextID() const
		{
			return std::find_if(species_.begin(), species_.end(),
				[](const std::string& s)
				{
					return s.empty();
				}
			) - species_.begin();
		}
	};
}