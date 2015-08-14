#include "ForceField.h"
#include "ForceFieldManager.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "LennardJonesFF.h"

using namespace Json;

namespace SAPHRON
{
	ForceField* ForceField::Build(const Json::Value& json, ForceFieldManager* ffm)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		ForceField* ff = nullptr;

		// Get forcefield type.
		std::string type = json.get("type", "none").asString(); 
		
		if(type == "LennardJones")
		{
			reader.parse(JsonSchema::LennardJones, schema);
			validator.Parse(schema, "#/forcefields");

			// Validate inputs. 
			validator.Validate(json, "#/forcefields");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double sigma = json["sigma"].asDouble();
			double rcut = json["rcut"].asDouble();
			std::string p1type = json["species"][0].asString();
			std::string p2type = json["species"][1].asString();
			ff = new LennardJonesFF(eps, sigma, rcut);

			try{
				ffm->AddNonBondedForceField(p1type, p2type, *ff);
			} catch(std::exception& e) {
				delete ff;
				throw BuildException({
					e.what()
				});
			}
		}
		else
		{
			throw BuildException({"Unknown forcefield type specified."});
		}

		return ff;
	}
}