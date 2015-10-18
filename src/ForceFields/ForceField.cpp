#include "ForceField.h"
#include "ForceFieldManager.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "LennardJonesFF.h"
#include "LebwohlLasherFF.h"
#include "DSFFF.h"

using namespace Json;

namespace SAPHRON
{
	ForceField* ForceField::BuildElectrostatic(const Value &json, ForceFieldManager *ffm)
	{
		return BuildElectrostatic(json, ffm, "#/forcefields/electrostatic");
	}

	ForceField* ForceField::BuildElectrostatic(const Value &json, ForceFieldManager *ffm, const std::string &path)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		ForceField* ff = nullptr;

		// Get forcefield type.
		std::string type = json.get("type", "none").asString(); 
		if(type == "DSF")
		{
			reader.parse(JsonSchema::DSFFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double alpha = json["alpha"].asDouble();
			
			ff = new DSFFF(alpha);
		}
		else
		{
			throw BuildException({path + ": Unknown forcefield type specified."});
		}

		// Add to appropriate species pair.
		try{
			std::string p1type = json["species"][0].asString();
			std::string p2type = json["species"][1].asString();
			ffm->AddElectrostaticForceField(p1type, p2type, *ff);
		} catch(std::exception& e) {
			delete ff;
			throw BuildException({
				e.what()
			});
		}

		return ff;
	}

	ForceField* ForceField::BuildNonBonded(const Json::Value& json, SAPHRON::ForceFieldManager *ffm)
	{
		return BuildNonBonded(json, ffm, "#/forcefields/nonbonded");
	}

	ForceField* ForceField::BuildNonBonded(const Json::Value& json, ForceFieldManager* ffm, const std::string& path)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		ForceField* ff = nullptr;

		// Get forcefield type.
		std::string type = json.get("type", "none").asString(); 
		
		if(type == "LennardJones")
		{
			reader.parse(JsonSchema::LennardJonesFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double sigma = json["sigma"].asDouble();
			
			ff = new LennardJonesFF(eps, sigma);
		}
		else if(type == "LebwohlLasher")
		{
			reader.parse(JsonSchema::LebwholLasherFF, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double gamma = json["gamma"].asDouble();
			ff = new LebwohlLasherFF(eps, gamma);
		}
		else
		{
			throw BuildException({path + ": Unknown forcefield type specified."});
		}

		// Add to appropriate species pair.
		try{
			std::string p1type = json["species"][0].asString();
			std::string p2type = json["species"][1].asString();
			ffm->AddNonBondedForceField(p1type, p2type, *ff);
		} catch(std::exception& e) {
			delete ff;
			throw BuildException({
				e.what()
			});
		}

		return ff;
	}

	void ForceField::BuildForceFields(const Value& json, ForceFieldManager* ffm, FFList& fflist)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::ForceFields, schema);
		validator.Parse(schema, "#/forcefields");

		// Validate high level schema.
		validator.Validate(json, "#/forcefields");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through non-bonded.
		int i = 0;
		for(auto& ff : json["nonbonded"]) 
		{
			fflist.push_back(BuildNonBonded(ff, ffm, "#/forcefields/nonbonded/" + std::to_string(i)));
			++i;
		}

		i = 0;
		for(auto& ff : json["electrostatic"])
		{
			fflist.push_back(BuildElectrostatic(ff, ffm, "#forcefields/electrostatic/" + std::to_string(i)));
			++i;
		}
	}
}