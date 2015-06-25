#include "World.h"
#include "SimpleWorld.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include <time.h>
#include <cstdlib>

using namespace Json;

namespace SAPHRON
{
	World* World::BuildWorld(std::istream& stream)
	{
		Reader reader;
		Value root;
		if(!reader.parse(stream, root))
			throw BuildException({reader.getFormattedErrorMessages()});

		return BuildWorld(root);
	}

	World* World::BuildWorld(const Value& json)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		World* world = nullptr;

		// Validation success.
		if(json["type"].asString() == "Simple")
		{
			// Parse schema.
			reader.parse(JsonSchema::SimpleWorld, schema);
			validator.Parse(schema, "#/worlds");

			// Validate input.
			validator.Validate(json, "#/worlds");

			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			Position dim{json["dimensions"][0].asDouble(), 
						 json["dimensions"][1].asDouble(),
						 json["dimensions"][2].asDouble()};

			// Neighbor list cutoff check.
			double ncut = json["nlist_cutoff"].asDouble();
			if(ncut > dim.x/2.0 || ncut > dim.y/2.0 || ncut > dim.z/2.0)
				throw BuildException({"Neighbor list cutoff must not exceed "
									  "half the shortest box vector"});
			
			// Skin thickness check.
			double skin = json["skin_thickness"].asDouble();
			if(skin > ncut)
				throw BuildException({"Skin thickness must not exceed neighbor list cutoff"});

			srand(time(NULL));
			int seed = json.isMember("seed") ? json["seed"].asInt() : rand();

			
			world = new SimpleWorld(dim.x, dim.y, dim.z, ncut, seed);
			world->SetSkinThickness(skin);
		}

		return world;
	}
}