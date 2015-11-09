#include <cstdlib>
#include <time.h>
#include "Move.h"
#include "MoveManager.h"
#include "json/json.h"
#include "schema.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "../Simulation/SimException.h"
#include "../Worlds/WorldManager.h"
#include "FlipSpinMove.h"
#include "TranslateMove.h"
#include "DirectorRotateMove.h"
#include "ParticleSwapMove.h"
#include "RandomIdentityMove.h"
#include "RotateMove.h"
#include "SpeciesSwapMove.h"
#include "VolumeSwapMove.h"
#include "InsertParticleMove.h"
#include "DeleteParticleMove.h"

using namespace Json;

namespace SAPHRON
{
	Move* Move::BuildMove(const Json::Value &json, SAPHRON::MoveManager *mm, WorldManager* wm)
	{
		return BuildMove(json, mm, wm, "#/moves");
	}

	Move* Move::BuildMove(const Value &json, 
						  MoveManager *mm, 
						  WorldManager* wm, 
						  const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		Move* move = nullptr;

		// Get move type. 
		std::string type = json.get("type", "none").asString();

		if(type == "DeleteParticle")
		{
			reader.parse(JsonSchema::DeleteParticleMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();
			
			std::vector<std::string> species;
			for(auto& s : json["species"])
				species.push_back(s.asString());

			move = new DeleteParticleMove(species, seed);
		}
		else if(type == "DirectorRotate")
		{
			reader.parse(JsonSchema::DirectorRotateMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new DirectorRotateMove(seed);
		}
		else if(type == "FlipSpin")
		{
			reader.parse(JsonSchema::FlipSpinMove, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new FlipSpinMove(seed);
		}
		else if(type == "InsertParticle")
		{
			reader.parse(JsonSchema::InsertParticleMove, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();
			int scount = json["stash_count"].asInt();

			std::vector<std::string> species;
			for(auto& s : json["species"])
				species.push_back(s.asString());

			move = new InsertParticleMove(species, *wm, scount, seed);
		}
		else if(type == "ParticleSwap")
		{
			reader.parse(JsonSchema::ParticleSwapMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new ParticleSwapMove(seed);
		}
		else if(type == "RandomIdentity")
		{
			reader.parse(JsonSchema::RandomIdentityMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			std::vector<std::string> identities;
			for(auto& i : json["identities"])
				identities.push_back(i.asString());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new RandomIdentityMove(identities, seed);
		}
		else if(type == "Rotate")
		{
			reader.parse(JsonSchema::RotateMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double dmax = json["maxangle"].asDouble();

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new RotateMove(dmax, seed);
		}
		else if(type == "SpeciesSwap")
		{
			reader.parse(JsonSchema::SpeciesSwapMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new SpeciesSwapMove(seed);
		}
		else if(type == "Translate")
		{
			reader.parse(JsonSchema::TranslateMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double dx = json["dx"].asDouble();
			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new TranslateMove(dx, seed);
		}
		else if(type == "VolumeSwap")
		{
			reader.parse(JsonSchema::VolumeSwapMove, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double dv = json["dv"].asDouble();
			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();

			move = new VolumeSwapMove(dv, seed);
		}
		else
		{
			throw BuildException({path + ": Unknown move type specified."});
		}

		// Add to appropriate species pair.
		try{
				int weight = json.get("weight", 1).asUInt();
				mm->AddMove(move, weight);
			} catch(std::exception& e) {
				delete move;
				throw BuildException({
					e.what()
				});
		}

		return move;
	}

	void Move::BuildMoves(const Json::Value &json, 
						  SAPHRON::MoveManager *mm, 
						  WorldManager* wm, 
						  MoveList &mvlist)
	{
		ArrayRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::Moves, schema);
		validator.Parse(schema, "#/moves");

		// Validate high level schema.
		validator.Validate(json, "#/moves");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through moves.
		int i = 0;
		for(auto& m : json)
		{
			mvlist.push_back(BuildMove(m, mm, wm, "#/moves/" + std::to_string(i)));
			++i;
		}

	}
}