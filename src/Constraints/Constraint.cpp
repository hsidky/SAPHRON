#include "Constraint.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "../Simulation/SimException.h"
#include "DirectorRestrictionC.h"
#include "PasquaMembraneC.h"
#include "schema.h"

using namespace Json;

namespace SAPHRON
{
	Constraint* Constraint::BuildConstraint(const Json::Value& json, ForceFieldManager* ffm, WorldManager* wm)
	{
		return BuildConstraint(json, ffm, wm, "#/forcefields/constraints");
	}

	Constraint* Constraint::BuildConstraint(
		const Json::Value& json, 
		ForceFieldManager* ffm, 
		WorldManager* wm,
		const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		Constraint* c = nullptr; 

		// Get constraint type and world. 
		auto type = json.get("type", "none").asString();
		if(type == "DirectorRestriction")
		{
			reader.parse(JsonSchema::DirectorRestrictionC, schema);
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

			auto w = wm->GetWorld(json["world"].asInt());
			c = new DirectorRestrictionC(w, coeff, dir, index, lim);
		}
		else if(type == "PasquaMembrane")
		{
			reader.parse(JsonSchema::PasquaMembraneC, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Parse(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto epsilon = json["epsilon"].asDouble();
			auto d = json["d"].asDouble();
			auto ra = json["ra"].asDouble();
			auto rb = json["rb"].asDouble();
			auto za = json["za"].asDouble();
			auto zb = json["zb"].asDouble();
			auto neq = json["neq"].asDouble();
			auto npol = json["npol"].asDouble();

			auto w = wm->GetWorld(json["world"].asInt());
			c = new PasquaMembraneC(w, epsilon, d, ra, rb, za, zb, neq, npol);
		}
		else
		{
			throw BuildException({path + ": Unknown constraint type specified."});
		}

		// Add appropriate species. 
		try{
			auto wid = json["world"].asInt();
			ffm->AddConstraint(*wm->GetWorld(wid), c);
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
		WorldManager* wm,
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
			ccl.push_back(BuildConstraint(c, ffm, wm, "#/forcefields/constraints/" + std::to_string(i))
				);
			++i;
		}
	}
}