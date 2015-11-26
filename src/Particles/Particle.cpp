#include "Particle.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "Site.h"
#include "Molecule.h"
#include "schema.h"
#include <queue>
#include <set>

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
	void Particle::SetSpeciesID(int speciesID)
	{
		assert(speciesID < (int)_speciesList.size());
		_pEvent.SetOldSpecies(_speciesID);
		_speciesID = speciesID;
		_species = _speciesList[speciesID];
		_pEvent.species = 1;
		NotifyObservers();
	}

	void Particle::AddChild(Particle *child)
	{
		child->SetParent(this);
		child->SetWorld(_world);
		_children.push_back(child);
	
		this->_pEvent.SetChild(child);
		this->_pEvent.child_add = 1;

		UpdateCenterOfMass();

		// Add all the same observers for children.
		for(auto& o : _observers)
			child->AddObserver(o);

		// Fire event. 
		NotifyObservers();
	}

	void Particle::RemoveChild(Particle* particle)
	{
		auto it = std::remove(_children.begin(), _children.end(), particle);
		if(it != _children.end())
		{
			this->_pEvent.SetChild(particle);
			this->_pEvent.child_remove = 1;

			particle->ClearParent();
			particle->SetWorld(nullptr);
			
			for(auto& o : _observers)
				particle->RemoveObserver(o);
			_children.erase(it);
			
			UpdateCenterOfMass();
			
			// Fire event.
			NotifyObservers();
		}
	}

	void Particle::RemoveFromNeighbors()
	{
		// Remove particle from neighbor list. 
		for(auto& neighbor : _neighbors)
		{
			if(neighbor != NULL && neighbor != nullptr)
				neighbor->RemoveNeighbor(this);
		}

		for(auto& c : *this)
			c->RemoveFromNeighbors();
	}

	void Particle::RemoveFromBondedNeighbors()
	{
		for(auto& neighbor : _bondedneighbors)
		{
			if(neighbor != NULL && neighbor != nullptr)
				neighbor->RemoveBondedNeighbor(this);
		}

		for(auto& c : *this)
			c->RemoveFromBondedNeighbors();
	}

	void Particle::GetBlueprint(Json::Value& json) const
	{
		if(!HasChildren())
		{
			json["species"] = GetSpecies();
			json["charge"] = GetCharge();
			json["mass"] = GetMass();
		}
		else
		{
			// TODO: Find a better mechanism than set.
			std::set<Json::Value> bonds;
			Json::Value bond;
			for(int i = 0; i < (int)_children.size(); ++i)
			{
				auto& last = json["children"][i];
				_children[i]->GetBlueprint(last);
				auto i1 = _children[i]->GetChildIndex();
				for(auto& b : _children[i]->GetBondedNeighbors())
				{
					int i2 = b->GetChildIndex();
					bond[0] = std::min(i1, i2);
					bond[1] = std::max(i1, i2);
					bonds.insert(bond);
				}
			}

			// Fill json schema with unique elements in set.
			for(auto& b : bonds)
				json["bonds"].append(b);
		}
	}
			
	Particle* Particle::BuildParticle(const Json::Value& particles, 
									  const Json::Value& blueprints)
	{
		ArrayRequirement arrayvalid;
		ObjectRequirement objvalid;
		Value pschema, bschema; 
		Reader reader;

		Particle* particle = nullptr;

		if(particles[0].isArray())
			throw BuildException({"Composite objects are not yet supported."});

		// Parse schemas.
		reader.parse(JsonSchema::Site, pschema);
		arrayvalid.Parse(pschema, "#/particles");
		reader.parse(JsonSchema::Components, bschema);
		objvalid.Parse(bschema, "#/components");

		// Validate inputs. 
		arrayvalid.Validate(particles, "#/particles");
		if(arrayvalid.HasErrors())
				throw BuildException(arrayvalid.GetErrors());

		objvalid.Validate(blueprints, "#/components");
		if(objvalid.HasErrors())
			throw BuildException(objvalid.GetErrors());

		// Declare all variables of interest. 
		int id = particles[0].asInt();
		std::string species = particles[1].asString();
		std::string component = particles[2].asString();
		Position pos{particles[3][0].asDouble(), 
					 particles[3][1].asDouble(), 
					 particles[3][2].asDouble()};
		Director dir;
		if(particles.size() > 4)
			dir = {particles[4][0].asDouble(), 
				   particles[4][1].asDouble(), 
				   particles[4][2].asDouble()};

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
	
	void Particle::BuildParticles(const Json::Value& particles, 
								  const Json::Value& blueprints, 
								  const Json::Value& components,
								  ParticleList &pvector)
	{
		ArrayRequirement arrayvalid;
		ObjectRequirement objvalid;
		Value pschema, bschema, cschema; 
		Reader reader;

		pvector.clear();

		// Parse schemas.
		reader.parse(JsonSchema::Particles, pschema);
		arrayvalid.Parse(pschema, "#/particles");
		reader.parse(JsonSchema::Blueprints, bschema);
		arrayvalid.Parse(bschema, "#/blueprints");
		reader.parse(JsonSchema::Components, cschema);
		objvalid.Parse(cschema, "#/components");

		// Validate inputs via schemas. 
		arrayvalid.Validate(particles, "#/particles");
		if(arrayvalid.HasErrors())
			throw BuildException(arrayvalid.GetErrors());

		objvalid.Validate(blueprints, "#/blueprints");
		if(objvalid.HasErrors())
			throw BuildException(objvalid.GetErrors());

		objvalid.Validate(components, "#/components");
		if(objvalid.HasErrors())
			throw BuildException(objvalid.GetErrors());

		// Perform semantic validation first, then initialize all particles. 
		// This is to avoid having to free memory on error(s).
		std::vector<int> uid;
		uid.reserve(particles.size() + 1);

		int i = 0;
		// Verify that components have blueprints and that 
		// particles in the particles array match up with the 
		// blueprint and component counts.
		for(auto& ctype : components.getMemberNames())
		{
			auto& component = components[ctype];
			if(!blueprints.isMember(ctype))
				throw BuildException({"Particle type \"" + 
					ctype + "\" not delcared in blueprints."});	
			auto& blueprint = blueprints[ctype];

			// Get parent particle counts.
			int count = component["count"].asInt();
			// Get potential children counts.
			int ccount = blueprint.isMember("children") ? blueprint["children"].size() : 0;
			// Compute actual number of particles.
			int pcount = ccount ? count * ccount : count;

			// Verify bond indices do not exceed children indices.
			if(blueprint.isMember("bonds"))
			{
				for(auto& bond : blueprint["bonds"])
				{
					if(bond[0].asInt() > ccount - 1 || bond[1].asInt() > ccount - 1)
						throw BuildException({
							"Particle \"" + ctype +  
							"\": bonded particle ID exceeds number of children."
						});
				}
			}

			// Loop through particles and validate type and order.
			for(int j = i; j < i + pcount; ++j)
			{
				if((i + pcount) > (int)particles.size())
					throw BuildException({"Component \"" + ctype + "\" count mismatch: " 
						"expecting " + std::to_string(i + pcount) + " particles."});

				// Get particle.
				auto& p = particles[j];
				auto id = p[0].asInt();
				auto species = p[1].asString();

				// Make sure particle ID is unique.
				if(std::find(uid.begin(), uid.end(), id) != uid.end())
					throw BuildException(
						{"Particle " + std::to_string(id) + ": must have unique ID."}
					);
				uid.push_back(id);

				// Validate species by determining what the current component should be.
				if(ccount) // component has child.
				{
					int sindex = (j - i) % ccount;
					auto child = blueprint["children"][sindex]["species"].asString();
					if(species != child)
						throw BuildException(
							{
								"Particle " + std::to_string(id) + ": expected type \"" 
								+ child + "\"."
							}
						);
				}
				else
					if(species != ctype)
						throw BuildException(
							{"Particle " + std::to_string(id) + 
							": expected type \"" + ctype + "\"."}
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
			Position pos{p[2][0].asDouble(), p[2][1].asDouble(), p[2][2].asDouble()};
			Director dir;
			if(p.size() > 3)
				dir = {p[3][0].asDouble(), p[3][1].asDouble(), p[3][2].asDouble()};

			// Create the particle.
			Particle* particle = new Site(pos, dir, species);
			particle->SetGlobalIdentifier(id);
			pcontainer.push(particle);
		}

		// Create parent particles and add to output container.
		// Here we also assign charges and masses from blueprints.
		for(auto& component : components.getMemberNames())
		{
			auto& spec = blueprints[component];
			int count = components[component]["count"].asInt();
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
					Particle* parent = new Molecule(component);
					for(int k = 0; k < ccount; ++k)
					{
						auto* p = pcontainer.front();

						p->SetMass(spec["children"][k].get("mass", 1.0).asDouble());
						p->SetCharge(spec["children"][k].get("charge", 0.0).asDouble());
						
						parent->AddChild(p);
						pcontainer.pop();
					}

					// Add bonded neighbor(s).
					auto& children = parent->GetChildren();
					for(auto& bond : spec["bonds"])
					{
						auto b1 = bond[0].asUInt();
						auto b2 = bond[1].asUInt();
						children[b1]->AddBondedNeighbor(children[b2]);
						children[b2]->AddBondedNeighbor(children[b1]);
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
	ParticleMap Particle::_identityList {};
	int Particle::_nextID = 0;
}
