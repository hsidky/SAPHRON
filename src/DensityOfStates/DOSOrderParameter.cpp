#include "DOSOrderParameter.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "../Simulation/Simulation.h"
#include "json/json.h"
#include "../Simulation/SimException.h"
#include "../Worlds/WorldManager.h"
#include "WangLandauOP.h"
#include "ElasticCoeffOP.h"

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
		if(type == "WangLandau")
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

			// Get x interval and check bounds.
			auto xmin = json["xrange"][0].asDouble();
			auto xmax = json["xrange"][1].asDouble();
			if(xmin > xmax)
				throw BuildException({"#orderparameter/xrange: xmin cannot exceed xmax."});

			auto boxvec = w->GetBoxVectors();
			if(xmin < 0 || xmax > boxvec[0])
				throw BuildException({"#orderparameter/xrange: xmin and xmax must be within world."});

			// Write filter function for proper deformation coordinate.
			EFilterFunc filter;
			auto mode = json["mode"].asString();
			if(mode == "twist")
			{
				filter = [=](const Particle* p) -> bool {
					auto& pos = p->GetPositionRef();
					return pos[0] >= xmin && pos[0] <= xmax;
				};
			}

			// Compute midpoint for deformation. This assumes 
			// that the last x "layer" is anchored (at x = xmax). 
			auto mid = boxvec[0] - 0.5*(xmax - xmin);

			// Initialize order parameter.
			op = new ElasticCoeffOP(*hist, *w, mid, filter);
		}
		else
		{
			throw BuildException({"#orderparameter: Unkown order parameter specified."});
		}

		return op;
	}
}