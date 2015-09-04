#include <cstdlib>
#include <time.h>
#include "Move.h"
#include "MoveManager.h"
#include "json/json.h"
#include "schema.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "../Simulation/SimException.h"
#include "FlipSpinMove.h"
#include "TranslateMove.h"
#include "DirectorRotateMove.h"

using namespace Json;

namespace SAPHRON
{
	Move* Move::BuildMove(const Json::Value &json, SAPHRON::MoveManager *mm)
	{
		return BuildMove(json, mm, "#/moves");
	}

	Move* Move::BuildMove(const Value &json, MoveManager *mm, const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		Move* move = nullptr;

		// Get move type. 
		std::string type = json.get("type", "none").asString();

		if(type == "FlipSpin")
		{
			reader.parse(JsonSchema::FlipSpinMove, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			move = new FlipSpinMove();
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
		else
		{
			throw BuildException({path + ": Unknown move type specified."});
		}

		// Add to appropriate species pair.
		try{
				mm->PushMove(*move);
			} catch(std::exception& e) {
				delete move;
				throw BuildException({
					e.what()
				});
		}

		return move;
	}

	void Move::BuildMoves(const Json::Value &json, SAPHRON::MoveManager *mm, MoveList &mvlist)
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
			mvlist.push_back(BuildMove(m, mm, "#/moves/" + std::to_string(i)));
			++i;
		}

	}
}