#include "DOSOrderParameter.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "../Simulation/Simulation.h"
#include "json/json.h"
#include "../Simulation/SimException.h"
#include "../Worlds/WorldManager.h"
#include "WangLandauOP.h"
#include "ElasticCoeffOP.h"
#include "ParticleDistanceOP.h"
#include "RgOP.h"
#include "ChargeFractionOP.h"

using namespace Json; 

namespace SAPHRON
{
	DOSOrderParameter* DOSOrderParameter::Build(const Value& json, Histogram* hist, WorldManager* wm)
	{
		ObjectRequirement validator;
		Value schema; 
		Reader reader; 

		if(hist == nullptr || hist == NULL)
			throw BuildException(
				{"#/orderparameter: Order parameter requires initialized histogram."}
			);


		DOSOrderParameter* op = nullptr;

		auto type = json.get("type", "none").asString();
		if(type == "ParticleDistance")
		{
			reader.parse(JsonSchema::ParticleDistanceOP, schema);
			validator.Parse(schema, "#/orderparameter");

			// Validate inputs. 
			validator.Validate(json, "#/orderparameter");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			ParticleList group1, group2;
			auto& plist = Particle::GetParticleMap();

			for(auto& id : json["group1"])
			{
				if(plist.find(id.asInt()) == plist.end())
					throw BuildException({"#orderparameter/group1 : unknown particle ID " + id.asString()});
				group1.push_back(plist[id.asInt()]);				
			}

			for(auto& id : json["group2"])
			{
				if(plist.find(id.asInt()) == plist.end())
					throw BuildException({"#orderparameter/group1 : unknown particle ID " + id.asString()});
				group2.push_back(plist[id.asInt()]);				
			}

			op = new ParticleDistanceOP(*hist, group1, group2);
		}
		else if(type == "WangLandau")
		{
			reader.parse(JsonSchema::WangLandauOP, schema);
			validator.Parse(schema, "#/orderparameter");

			// Validate inputs. 
			validator.Validate(json, "#/orderparameter");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			op = new WangLandauOP(*hist);
		}
		else if(type == "ElasticCoeff")
		{
			reader.parse(JsonSchema::ElasticCoeffOP, schema);
			validator.Parse(schema, "#/orderparameter");

			// Validate inputs. 
			validator.Validate(json, "#/orderparameter");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			// Get world 
			auto windex = json.get("world", 0).asInt();
			if(windex > (int)wm->GetWorldCount())
				throw BuildException({"#/orderparameter/world: "
					"Specified index exceeds actual count."
				});

			auto* w = wm->GetWorld(windex);

			// Get range and check bounds.
			auto rmin = json["range"][0].asDouble();
			auto rmax = json["range"][1].asDouble();
			if(rmin > rmax)
				throw BuildException({"#orderparameter/range: rmin cannot exceed rmax."});

			// Get mode.
			auto mode = json["mode"].asString();
			ElasticMode elmode;
			if(mode == "splay")
				elmode = Splay;
			else if(mode == "twist")
				elmode = Twist;
			else
				elmode = Bend;

			// TODO: Generalize (in particular lambda fxn) for
			// many geometries.
			const auto& boxvec = w->GetHMatrix();

			// Get length of appropriate dimension.
			auto rlen = (elmode == Bend) ? boxvec(2,2) : boxvec(0,0);
			if(rmin < 0 || rmax > rlen)
				throw BuildException({"#orderparameter/range: rmin and rmax must be within world."});

			// Compute midpoint for deformation. This assumes 
			// that the last "layer" is anchored.
			double mid = rlen - 0.5*(rmax + rmin);

			// Initialize order parameter.
			op = new ElasticCoeffOP(*hist, w, mid, {{rmin, rmax}}, elmode);
		}
		else if(type == "ChargeFraction")
		{
			reader.parse(JsonSchema::ChargeFractionOP, schema);
			validator.Parse(schema, "#/orderparameter");

			// Validate inputs. 
			validator.Validate(json, "#/orderparameter");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			ParticleList group1;
			auto& plist = Particle::GetParticleMap();

			for(auto& id : json["group1"])
			{
				if(plist.find(id.asInt()) == plist.end())
					throw BuildException({"#orderparameter/group1 : unknown particle ID " + id.asString()});
				group1.push_back(plist[id.asInt()]);				
			}

			double charge = json.get("Charge",0).asDouble();

			// Initialize order parameter.
			op = new ChargeFractionOP(*hist, group1, charge);
		}
		else if(type == "Rg")
		{
			reader.parse(JsonSchema::RgOP, schema);
			validator.Parse(schema, "#/orderparameter");

			// Validate inputs. 
			validator.Validate(json, "#/orderparameter");
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			ParticleList group1;
			auto& plist = Particle::GetParticleMap();

			for(auto& id : json["group1"])
			{
				if(plist.find(id.asInt()) == plist.end())
					throw BuildException({"#orderparameter/group1 : unknown particle ID " + id.asString()});
				group1.push_back(plist[id.asInt()]);				
			}

			// Initialize order parameter.
			op = new RgOP(*hist, group1);
		}
		else
		{
			throw BuildException({"#orderparameter: Unkown order parameter specified."});
		}

		return op;
	}
}