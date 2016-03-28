#include "BlueprintManager.h"
#include "../Validator/ObjectRequirement.h"
#include "../Simulation/SimException.h"
#include "schema.h"

using namespace Json;

namespace SAPHRON
{
	void BlueprintManager::Build(const Value& json)
	{
		ObjectRequirement validator;
		Value schema; 
		Reader reader;

		reader.parse(JsonSchema::Blueprints, schema);
		validator.Parse(schema, "#/blueprints");

		validator.Validate(json, "#/blueprints");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Get instance of self. 
		auto& bp = BlueprintManager::Instance();

		// Go through blueprints.
		for(auto& species : json.getMemberNames())
		{
			std::vector<Site> sites;
			std::vector<uint> indices;
			auto& p = json[species];
			for(auto& s : p["sites"])
			{
				Site site;
				site.position = {
					s["position"][0].asDouble(), 
					s["position"][1].asDouble(), 
					s["position"][2].asDouble()
				};

				if(s.isMember("director"))
				{
					site.director = {
						s["director"][0].asDouble(),
						s["director"][1].asDouble(),
						s["director"][2].asDouble()
					};
				}

				site.charge = s.get("charge", 0.).asDouble();
				site.mass = s.get("mass", 1.).asDouble();

				auto ss = s["species"].asString();

				if(!bp.IsRegistered(ss))
				{
					site.species = bp.GetNextID();
					bp.AddSpeciesIndex(ss, site.species);
				}
				else
					site.species = bp.GetSpeciesIndex(ss);

				sites.push_back(site);
				indices.push_back(indices.size());
			}

			NewParticle particle(bp.GetNextID(), indices, &sites);
			bp.AddBlueprint(species, particle);
		}
	}
}