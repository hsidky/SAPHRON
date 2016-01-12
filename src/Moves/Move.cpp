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
#include "TranslatePrimitiveMove.h"
#include "DirectorRotateMove.h"
#include "ParticleSwapMove.h"
#include "RandomIdentityMove.h"
#include "RotateMove.h"
#include "SpeciesSwapMove.h"
#include "VolumeSwapMove.h"
#include "VolumeScaleMove.h"
#include "InsertParticleMove.h"
#include "DeleteParticleMove.h"
#include "AnnealChargeMove.h"
#include "AcidTitrationMove.h"
#include "AcidReactionMove.h"

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

		if(type == "AcidReaction")
		{
			reader.parse(JsonSchema::AcidReactionMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();	

			std::vector<std::string> reactants;
			for(auto& s : json["reactants"])
				reactants.push_back(s.asString());

			std::vector<std::string> products;
			for(auto& s : json["products"])
				products.push_back(s.asString());

			auto protoncharge = json.get("proton_charge", 1.0).asDouble();
			auto pKo = json.get("pKo", 0.0).asDouble();
			auto scount = json["stash_count"].asInt();

			auto prefac = json.get("op_prefactor", true).asBool();

			auto* m = new AcidReactionMove(reactants,products,*wm,
			scount, pKo, protoncharge, seed);
			m->SetOrderParameterPrefactor(prefac);
			move = static_cast<Move*>(m);
		}
		else if(type == "AcidTitrate")
		{
			reader.parse(JsonSchema::AcidTitrateMove, schema);
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

			auto protoncharge = json.get("proton_charge", 1.0).asDouble();
			auto mu = json.get("mu", 0.0).asDouble();
			auto prefac = json.get("op_prefactor", true).asBool();

			auto* m = new AcidTitrationMove(species, protoncharge, mu, seed);
			m->SetOrderParameterPrefactor(prefac);
			move = static_cast<Move*>(m);
		}
		else if(type == "AnnealCharge")
		{
			reader.parse(JsonSchema::AnnealChargeMove, schema);
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

			move = new AnnealChargeMove(species, seed);
		}
		else if(type == "DeleteParticle")
		{
			reader.parse(JsonSchema::DeleteParticleMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();
			auto prefac = json.get("op_prefactor", true).asBool();
			
			std::vector<std::string> species;
			for(auto& s : json["species"])
				species.push_back(s.asString());

			auto* m = new DeleteParticleMove(species, seed);
			m->SetOrderParameterPrefactor(prefac);
			move = static_cast<Move*>(m);
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
			auto seed = json.get("seed", rand()).asInt();
			auto scount = json["stash_count"].asInt();
			auto prefac = json.get("op_prefactor", true).asBool(); 

			std::vector<std::string> species;
			for(auto& s : json["species"])
				species.push_back(s.asString());

			auto* m = new InsertParticleMove(species, *wm, scount, seed);
			m->SetOrderParameterPrefactor(prefac);
			move = static_cast<Move*>(m);
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
		
			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();
			if(json["dx"].isObject())
			{
				std::map<std::string, double> dx; 
				for(auto& s : json["dx"].getMemberNames())
					dx[s] = json["dx"][s].asDouble();

				auto expl = json.get("explicit_draw", false).asBool();

				move = new TranslateMove(dx, expl, seed);
			}
			else
			{
				auto dx = json["dx"].asDouble();
				move = new TranslateMove(dx, seed);
			}
		}
		else if(type == "TranslatePrimitive")
		{
			reader.parse(JsonSchema::TranslatePrimitiveMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
		
			srand(time(NULL));
			int seed = json.get("seed", rand()).asInt();
			if(json["dx"].isObject())
			{
				std::map<std::string, double> dx; 
				for(auto& s : json["dx"].getMemberNames())
					dx[s] = json["dx"][s].asDouble();

				auto expl = json.get("explicit_draw", false).asBool();

				move = new TranslatePrimitiveMove(dx, expl, seed);
			}
			else
			{
				auto dx = json["dx"].asDouble();
				move = new TranslatePrimitiveMove(dx, seed);
			}
		}
		else if(type == "VolumeScale")
		{
			reader.parse(JsonSchema::VolumeScaleMove, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto dv = json["dv"].asDouble();
			auto Pextern = json["Pextern"].asDouble();
			srand(time(NULL));
			auto seed = json.get("seed", rand()).asInt();

			move = new VolumeScaleMove(Pextern, dv, seed);		
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