#pragma once 

#include "NewParticle.h"
#include <memory>

namespace SAPHRON
{
	// Manages particle blueprints.
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

		// Add a blueprint to the blueprint manager and associate with species string. 
		// The species id inside the particle will be used to index the string. Will 
		// override existing blueprint if one already exists for that species string and 
		// will associate a new integer.
		void AddBlueprint(const std::string& species, const NewParticle& p)
		{
			auto n = p.GetSpecies();
			if(n >= species_.size())
			{
				species_.resize(n+1);
				particles_.resize(n+1);
			}

			particles_[n].reset();
			particles_[n] = std::unique_ptr<NewParticle>(new NewParticle(p, sites_));
			species_[n] = species;
		}

		// Removes a blueprint from the blueprint manager.
		void RemoveBlueprint(const std::string& species)
		{
			auto id = GetSpeciesIndex(species);
			particles_[id].reset();
		}

		// Gets a copy of a particle from the blueprint storage. Vector must be 
		// passed in to be filled with sites.
		NewParticle GetBlueprint(const std::string& species, std::vector<Site>& sites)
		{
			auto id = GetSpeciesIndex(species);
			return {*particles_[id], sites};
		}

		// Gets a copy of a particle from the blueprint storage. Vector must be 
		// passed in to be filled with sites.
		NewParticle GetBlueprint(uint species, std::vector<Site>& sites)
		{
			return {*particles_[species], sites};
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
			return species_.size();
		}

		// Gets the number of registered blueprints.
		uint GetBlueprintCount() const
		{
			return std::count_if(particles_.begin(), particles_.end(), [](const std::unique_ptr<NewParticle>& p){
				return !(!p);
			});
		}

		// Get string associated with species ID.
		std::string GetSpeciesString(uint id)
		{
			return species_[id];
		}
	};
}