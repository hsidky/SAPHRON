#include "ForceField.h"
#include "ForceFieldManager.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "DebyeHuckelFF.h"
#include "DSFFF.h"
#include "FENEFF.h"
#include "GayBerneFF.h"
#include "HardSphereFF.h"
#include "HarmonicFF.h"
#include "KernFrenkelFF.h"
#include "LennardJonesFF.h"
#include "LennardJonesTSFF.h"
#include "LebwohlLasherFF.h"
#include "ModLennardJonesTSFF.h"
#include "EwaldFF.h"

using namespace Json;

namespace SAPHRON
{
	ForceField* ForceField::BuildElectrostatic(const Value &json, 
											   ForceFieldManager *ffm)
	{
		return BuildElectrostatic(json, ffm, "#/forcefields/electrostatic");
	}

	ForceField* ForceField::BuildElectrostatic(const Value &json, 
											   ForceFieldManager *ffm, 
											   const std::string &path)
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

			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());
			
			ff = new DSFFF(alpha, rc);
		}
		else if(type == "Ewald")
		{
			reader.parse(JsonSchema::EwaldFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double alpha = json["alpha"].asDouble();
			double kmaxx = json["kmax"][0].asInt();
			double kmaxy = json["kmax"][1].asInt();
			double kmaxz = json["kmax"][2].asInt();

			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());
			
			ff = new EwaldFF(alpha, kmaxx, kmaxy, kmaxz, rc);
		}
		else if(type == "DebyeHuckel")
		{
			reader.parse(JsonSchema::DebyeHuckelFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double kappa = json["kappa"].asDouble();


			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());
			
			
			ff = new DebyeHuckelFF(kappa, rc);
		}
		else
		{
			throw BuildException({path + ": Unknown electrostatic forcefield type specified."});
		}

		// Add to appropriate species pair.
		try{
			ffm->SetElectrostaticForcefield(*ff);
		} catch(std::exception& e) {
			delete ff;
			throw BuildException({
				e.what()
			});
		}

		return ff;
	}

	ForceField* ForceField::BuildNonBonded(const Json::Value& json, 
										   SAPHRON::ForceFieldManager *ffm)
	{
		return BuildNonBonded(json, ffm, "#/forcefields/nonbonded");
	}

	ForceField* ForceField::BuildNonBonded(const Json::Value& json, 
										   ForceFieldManager* ffm, 
										   const std::string& path)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		ForceField* ff = nullptr;

		// Get forcefield type.
		std::string type = json.get("type", "none").asString(); 
		
		if(type == "HardSphere")
		{
			reader.parse(JsonSchema::HardSphereFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto sigma = json["sigma"].asDouble();
			
			ff = new HardSphereFF(sigma);
		}
		else if(type == "LennardJones")
		{
			reader.parse(JsonSchema::LennardJonesFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double sigma = json["sigma"].asDouble();
			
			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());

			ff = new LennardJonesFF(eps, sigma, rc);
		}
		else if(type == "LennardJonesTS")
		{
			reader.parse(JsonSchema::LennardJonesTSFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double sigma = json["sigma"].asDouble();
			
			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());

			ff = new LennardJonesTSFF(eps, sigma, rc);
		}
		else if(type == "LebwohlLasher")
		{
			reader.parse(JsonSchema::LebwohlLasherFF, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double gamma = json["gamma"].asDouble();
			ff = new LebwohlLasherFF(eps, gamma);
		}
		else if(type == "ModLennardJonesTS")
		{
			reader.parse(JsonSchema::ModLennardJonesTSFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double eps = json["epsilon"].asDouble();
			double sigma = json["sigma"].asDouble();
			double beta = json["beta"].asDouble();
			
			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());

			ff = new ModLennardJonesTSFF(eps, sigma, beta, rc);
		}
		else if(type == "GayBerne")
		{
			reader.parse(JsonSchema::GayBerneFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto di = json["diameters"][0].asDouble();
			auto dj = json["diameters"][1].asDouble();
			auto li = json["lengths"][0].asDouble();
			auto lj = json["lengths"][1].asDouble();
			auto eps0 = json["eps0"].asDouble();
			auto epsE = json["epsE"].asDouble();
			auto epsS = json["epsS"].asDouble();
			auto dw = json.get("dw", 1.0).asDouble();
			auto mu = json.get("mu", 2.0).asDouble();
			auto nu = json.get("nu", 1.0).asDouble();

			CutoffList rc;
			for(auto r : json["rcut"])
				rc.push_back(r.asDouble());

			ff = new GayBerneFF(di, dj, li, lj, eps0, epsE, epsS, dw, rc, mu, nu);
		}
		else if(type == "KernFrenkel")
		{
			reader.parse(JsonSchema::KernFrenkelFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto eps = json["epsilon"].asDouble();
			auto sig = json["sigma"].asDouble();
			auto del = json["delta"].asDouble();
			auto inv = json.get("invert", false).asBool();

			if(json["thetas"].size() != json["pjs"].size())
				throw BuildException({"There must be an equal number of thetas and patch vectors."});

			std::vector<double> thetas;
			for(auto& t : json["thetas"])
				thetas.push_back(t.asDouble());

			std::vector<Vector3D> pjs;
			for(auto& p : json["pjs"])
				pjs.push_back(Vector3D{p[0].asDouble(), p[1].asDouble(), p[2].asDouble()});

			ff = new KernFrenkelFF(eps, sig, del, thetas, pjs, inv);
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
	
	ForceField* ForceField::BuildBonded(const Json::Value& json, ForceFieldManager *ffm)
	{
		return BuildBonded(json, ffm, "#/forcefields/bonded");
	}

	ForceField* ForceField::BuildBonded(const Json::Value& json, 
										ForceFieldManager* ffm, 
										const std::string& path)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		ForceField* ff = nullptr;

		// Get forcefield type.
		std::string type = json.get("type", "none").asString(); 
		
		if(type == "FENE")
		{
			reader.parse(JsonSchema::FENEFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto eps = json["epsilon"].asDouble();
			auto sigma = json["sigma"].asDouble();
			auto kspring = json["kspring"].asDouble();
			auto rmax = json["rmax"].asDouble();

			ff = new FENEFF(eps, sigma, kspring, rmax);
		}
		else if(type == "Harmonic")
		{
			reader.parse(JsonSchema::HarmonicFF, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto kspring = json["kspring"].asDouble();
			auto ro = json["ro"].asDouble();

			ff = new Harmonic(kspring, ro);
		}
		else
		{
			throw BuildException({path + ": Unknown forcefield type specified."});
		}

		// Add to appropriate species pair.
		try{
			std::string p1type = json["species"][0].asString();
			std::string p2type = json["species"][1].asString();
			ffm->AddBondedForceField(p1type, p2type, *ff);
		} catch(std::exception& e) {
			delete ff;
			throw BuildException({
				e.what()
			});
		}

		return ff;		
	}

	void ForceField::BuildForceFields(const Value& json, 
									  ForceFieldManager* ffm, 
									  FFList& fflist)
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
			fflist.push_back(
				BuildNonBonded(ff, ffm, "#/forcefields/nonbonded/" + std::to_string(i))
				);
			++i;
		}

		// Set electrostatic.
		if(json.isMember("electrostatic"))
			fflist.push_back(
				BuildElectrostatic(json["electrostatic"], ffm, "#forcefields/electrostatic")
				);

		// Loop through bonded.
		i = 0;
		for(auto& ff : json["bonded"])
		{
			fflist.push_back(
				BuildBonded(ff, ffm, "#forcefields/bonded/" + std::to_string(i))
				);
			++i;
		}
	}
}