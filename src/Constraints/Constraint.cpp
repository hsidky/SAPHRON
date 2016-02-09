#include "Constraint.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "../Simulation/SimException.h"
#include "schema.h"
#include "Dynamic1DP2C.h"

using namespace Json;

namespace SAPHRON
{
	Constraint* Constraint::BuildConstraint(const Json::Value& json, ForceFieldManager* ffm)
	{
		return BuildConstraint(json, ffm, "#/forcefields/constraints");
	}

	Constraint* Constraint::BuildConstraint(
		const Json::Value& json, 
		ForceFieldManager* ffm, 
		const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		Constraint* c = nullptr; 

		// Get constraint type. 
		auto type = json.get("type", "none").asString();
		if(type == "Dynamic1DP2")
		{
			reader.parse(JsonSchema::Dynamic1DP2C, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto coeff = json["coefficient"].asDouble();
			Director dir{
				json["director"][0].asDouble(), 
				json["director"][1].asDouble(),
				json["director"][2].asDouble()};

			auto index = json["index"].asInt();

			std::array<double, 2> lim{{
				json["limits"][0].asDouble(),
				json["limits"][1].asDouble()}};

			// Make sure min < max. 
			if(lim[0] > lim[1])
				throw BuildException({path + ": Limit minimum cannot exceed maximum."});

			c = new Dynamic1DP2C(coeff, dir, index, lim);
		}
		else
		{
			throw BuildException({path + ": Unknown constraint type specified."});
		}

		// Add appropriate species. 
		try{
			auto species = json["species"].asString();
			ffm->AddConstraint(species, c);
		} catch(std::exception& e) {
			delete c; 
			throw BuildException({
				e.what()
			});
		}

		return c;
	}

	void Constraint::BuildConstraints(
		const Value& json, 
		ForceFieldManager* ffm, 
		ConstraintList& ccl)
	{
		ArrayRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::ForceFields, schema);
		validator.Parse(schema, "#/forcefields/constraints");

		// Validate high level schema.
		validator.Validate(json, "#/forcefields/constraints");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through constraints.
		int i = 0; 
		for(auto& c : json)
		{
			ccl.push_back(BuildConstraint(c, ffm, "#/forcefields/constraints/" + std::to_string(i))
				);
			++i;
		}
	}
}