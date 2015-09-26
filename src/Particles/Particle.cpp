#include "Particle.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "Site.h"
#include "Molecule.h"
#include "schema.h"
#include <queue>

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

	Particle* Particle::BuildParticle(const Json::Value& particles, const Json::Value& blueprints)
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
		if(blueprints.isMember(species) && blueprints[species].isMember("children"))
			throw BuildException(
				{"Particle " + std::to_string(id) + ": only primitive types can be declared."}
			);
		else if(!blueprints.isMember(component))
			throw BuildException(
				{"Particle " + std::to_string(id) + " does not match a component definition."}
			);

		// Create the particle.
		particle = new Site(pos, dir, species);
		particle->SetGlobalIdentifier(id);

		return particle;
	}
	
	void Particle::BuildParticles(const Json::Value &particles, 
								  const Json::Value &blueprints, 
								  ParticleList &pvector)
	{
		ArrayRequirement pvalidator;
		ObjectRequirement bvalidator;
		Value pschema, bschema; 
		Reader reader;

		pvector.clear();

		// Parse schemas.
		reader.parse(JsonSchema::Particles, pschema);
		pvalidator.Parse(pschema, "#/particles");
		reader.parse(JsonSchema::Components, bschema);
		bvalidator.Parse(bschema, "#/components");

		// Validate inputs via schemas. 
		pvalidator.Validate(particles, "#/particles");
		if(pvalidator.HasErrors())
			throw BuildException(pvalidator.GetErrors());

		bvalidator.Validate(blueprints, "#/components");
		if(bvalidator.HasErrors())
			throw BuildException(bvalidator.GetErrors());

		// Perform semantic validation first, then initialize all particles. 
		// This is to avoid having to free memory on error(s).
		std::vector<int> uid;

		// Verify particle counts.			
		int i = 0;
		for(auto& bp : blueprints.getMemberNames())
		{
			int count = blueprints[bp]["count"].asInt();
			int ccount = blueprints[bp].isMember("children") ? blueprints[bp]["children"].size() : 0;
			int pcount = ccount ? count * ccount : count;

			// Loop though particles and validate type and order.
			for(int j = i; j < i + pcount; ++j)
			{
				if((i + pcount) > (int)particles.size())
					throw BuildException({"Component \"" + bp + "\" count mismatch: " 
						"expecting " + std::to_string(i + pcount) + " particles."});

				auto& p = particles[j];

				int id = p[0].asInt();
				std::string species = p[1].asString();
				std::string component = p[2].asString();
				
				// Make sure ID is unique.
				if(std::find(uid.begin(), uid.end(), id) != uid.end())
					throw BuildException(
						{"Particle " + std::to_string(id) + ": must have unique ID."}
					);
				uid.push_back(id);

				// Validate component.
				if(component != bp)
					throw BuildException(
						{"Particle " + std::to_string(id) + ": must belong to component \"" + bp + "\"."}
					);

				// Validate species by determining what the current component should be.
				if(ccount) // component has child.
				{
					int sindex = (j - i) % ccount;
					auto& children = blueprints[bp]["children"];
					if(species != children[sindex]["species"].asString())
						throw BuildException(
							{
								"Particle " + std::to_string(id) + ": expected type \"" 
								+ children[sindex]["species"].asString() + "\"."
							}
						);
				}
				else
					if(species != bp)
						throw BuildException(
							{"Particle " + std::to_string(id) + ": expected type \"" + bp + "\"."}
						);
			}
			i += pcount;
		}

		if(i != (int)particles.size())
			throw BuildException(
				{
					"Expected " + std::to_string(i) + " particles, but " + 
					std::to_string(particles.size()) + " specified."
				});
	
		// Loop through and create particles. First we initialize all primitive types,
		// then load them into parents if need be. We do this to ensure appropriate 
		// global ID assignment (they are auto-assigned on particle instantiation).
		std::queue<Particle*> pcontainer;

		for(auto& p : particles)
		{
			int id = p[0].asInt();
			std::string species = p[1].asString();
			std::string component = p[2].asString();
			Position pos{p[3][0].asDouble(), p[3][1].asDouble(), p[3][2].asDouble()};
			Director dir;
			if(p.size() > 4)
				dir = {p[4][0].asDouble(), p[4][1].asDouble(), p[4][2].asDouble()};

			// Create the particle.
			Particle* particle = new Site(pos, dir, species);
			particle->SetGlobalIdentifier(id);
			pcontainer.push(particle);
		}

		// Create parent particles and add to output container.
		// Here we also assign charges and masses from blueprints.
		for(auto& bp : blueprints.getMemberNames())
		{
			auto& spec = blueprints[bp];
			int count = spec["count"].asInt();
			int ccount = spec.isMember("children") ? spec["children"].size() : 0;

			for(int j = 0; j < count; ++j)
			{
				// No children. Particle is primitive.
				if(!ccount)
				{
					auto* p = pcontainer.front();
					p->SetMass(spec.get("mass", 1.0).asDouble());
					p->SetCharge(spec.get("charge", 0.0).asDouble());
					pvector.push_back(p);
					pcontainer.pop();
				}
				else
				{
					// Pop children.
					Particle* parent = new Molecule(bp);
					for(int k = 0; k < ccount; ++k)
					{
						auto* p = pcontainer.front();

						p->SetMass(spec[p->GetSpecies()].get("mass", 1.0).asDouble());
						p->SetCharge(spec[p->GetSpecies()].get("charge", 0.0).asDouble());
						
						parent->AddChild(p);
						pcontainer.pop();
					}
					pvector.push_back(parent);
				}
			}
		}

		if(pcontainer.size() != 0)
			throw BuildException({"Unknown error occurred: " + 
				std::to_string(pcontainer.size()) + " uninitialized particle(s)."});
	}


	SpeciesList Particle::_speciesList(0);
	std::map<int, Particle*> Particle::_identityList {};
	int Particle::_nextID = 0;
}
