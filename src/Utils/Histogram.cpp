#include "Histogram.h"
#include "json/json.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include "../Simulation/SimException.h"

using namespace Json;

namespace SAPHRON
{
	Histogram* Histogram::BuildHistogram(const Value& json)
	{
		ObjectRequirement validator;
		Value schema; 
		Reader reader;

		Histogram* hist = nullptr; 

		reader.parse(JsonSchema::Histogram, schema);
		validator.Parse(schema, "#/histogram");
		validator.Validate(json, "#/histogram");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Get properties 
		auto min = json.get("min", 0).asDouble();
		auto max = json.get("max", 0).asDouble();
		auto bincount = json.get("bincount", 0).asInt();
		auto binwidth = json.get("binwidth", 0).asDouble();

		// Semantic validation.
		if(min >= max)
			throw BuildException({"#/histogram/min: Minimum must be less than maximum."});

		if(bincount != 0 && binwidth != 0)
			throw BuildException({"#/histogram: Bin count and width cannot be specified simultaneously."});

		if(bincount == 0 && binwidth == 0)
			throw BuildException({"#/histogram: Either bin width or bin count must be specified."});

		// If user specifies bin width, compute bin count. 
		if(binwidth != 0)
			bincount = (int)ceil((max-min)/binwidth);

		// Load up values.
		std::vector<double> vals;
		vals.reserve(bincount);

		if(json.isMember("values"))
		{
			if((int)json.get("values",Json::arrayValue).size() != bincount)
				throw BuildException({"#/histogram/values: Expected " + std::to_string(bincount) + " values."});
			
			// Load up values.
			for(auto& v : json["values"])
				vals.push_back(v.asDouble());
		}

		// Load up counts. 
		std::vector<int> counts;
		counts.reserve(bincount);
		if(json.isMember("counts"))
		{
			if((int)json.get("counts", Json::arrayValue).size() != bincount)
				throw BuildException({"#/histogram/counts: Expected " + std::to_string(bincount) + " counts."});

			for(auto& c : json["counts"])
				counts.push_back(c.asInt());
		}

		// Create histogram. 
		hist = new Histogram(min, max, bincount);

		// Load values and counts. 
		for(size_t i = 0; i < vals.size(); ++i)
			hist->UpdateValue(i, vals[i]);
	
		for(size_t i = 0; i < counts.size(); ++i)
			hist->SetCount(i, counts[i]);

		return hist;
	}
}