#include "Particle.h"

namespace SAPHRON
{
	// Set the species of a particle.
	void Particle::SetSpecies(std::string species)
	{
		auto search = std::find(_speciesList.begin(), _speciesList.end(), species);
		if (search != _speciesList.end())
			_speciesID = search - _speciesList.begin();
		else
		{
			_speciesID = (int) _speciesList.size();
			_speciesList.push_back(species);
		}

		_species = species;
		_pEvent.species = 1;
		NotifyObservers();
	}

	// Set the species of the particle.
	void Particle::SetSpecies(int speciesID)
	{
		assert(speciesID < (int)_speciesList.size());
		_speciesID = speciesID;
		_species = _speciesList[speciesID];
		_pEvent.species = 1;
		NotifyObservers();
	}
	
	SpeciesList Particle::_speciesList;
	std::map<int, Particle*> Particle::_identityList;
	int Particle::_nextID = 0;
}
