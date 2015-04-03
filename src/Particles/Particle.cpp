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
	}

	// Set the species of the particle.
	void Particle::SetSpecies(int speciesID)
	{
		assert(speciesID < (int)_speciesList.size());
		_speciesID = speciesID;
		_species = _speciesList[speciesID];
	}

	SpeciesList Particle::_speciesList;
	int Particle::_nextID = 0;
}
