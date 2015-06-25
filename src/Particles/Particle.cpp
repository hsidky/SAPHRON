#include "Particle.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "Site.h"
#include "schema.h"

using namespace Json;

namespace SAPHRON
{
	// Set the species of a particle.
	void Particle::SetSpecies(std::string species)
	{
		_pEvent.SetOldSpecies(_speciesID);
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
		_pEvent.SetOldSpecies(_speciesID);
		_speciesID = speciesID;
		_species = _speciesList[speciesID];
		_pEvent.species = 1;
		NotifyObservers();
	}

	Particle* Particle::Build(const Json::Value& particles, const Json::Value& blueprints)
	{
		ArrayRequirement pvalidator;
		ObjectRequirement bvalidator;
		Value pschema, bschema; 
		Reader reader;

		Particle* particle = nullptr;

		if(particles[0].isArray())
			throw BuildException({"Composite objects are not yet supported."});

		// Parse schemas.
		reader.parse(JsonSchema::Site, pschema);
		pvalidator.Parse(pschema, "#/particles");
		reader.parse(JsonSchema::Components, bschema);
		bvalidator.Parse(bschema, "#/components");

		// Validate inputs. 
		pvalidator.Validate(particles, "#/particles");
		if(pvalidator.HasErrors())
				throw BuildException(pvalidator.GetErrors());

		bvalidator.Validate(blueprints, "#/components");
		if(bvalidator.HasErrors())
			throw BuildException(bvalidator.GetErrors());

		// Declare all variables of interest. 
		int id = particles[0].asInt();
		std::string species = particles[1].asString();
		std::string component = particles[2].asString();
		Position pos{particles[3][0].asDouble(), particles[3][1].asDouble(), particles[3][2].asDouble()};
		Director dir;
		if(particles.size() > 4)
			dir = {particles[4][0].asDouble(), particles[4][1].asDouble(), particles[4][2].asDouble()};

		// Check that we have a proper blueprint.
		if(blueprints.isMember(component) && blueprints[component].isMember("children"))
			throw BuildException({"Particle " + std::to_string(id) + ": only primitive types can be declared."});
		else if(!blueprints.isMember(component))
			throw BuildException({"Particle " + std::to_string(id) + " does not match a component definition."});

		// Create the particle.
		particle = new Site(pos, dir, species);
		particle->SetGlobalIdentifier(id);

		return particle;
	}
	
	SpeciesList Particle::_speciesList(0);
	std::map<int, Particle*> Particle::_identityList {};
	int Particle::_nextID = 0;
}
