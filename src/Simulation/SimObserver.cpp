#include "SimObservable.h"
#include "SimObserver.h"
#include "json/json.h"
#include "schema.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "SimException.h"
#include "../Observers/DLMFileObserver.h"
#include "../Observers/JSONObserver.h"
#include "../Observers/XYZObserver.h"

using namespace Json;

namespace SAPHRON
{
	// Initialized SimFlags based on (pre-validated) JSON.
	SimFlags InitFlags(const Value& json)
	{
		SimFlags flags;

		flags.iteration = json.get("iteration", 0).asUInt();
		flags.move_acceptances = json.get("move_acceptances", 0).asUInt();
		flags.dos_factor = json.get("dos_factor", 0).asUInt();
		flags.dos_flatness = json.get("dos_flatness", 0).asUInt();
		flags.world_pressure = json.get("world_pressure", 0).asUInt();
		flags.world_volume = json.get("world_volume", 0).asUInt();
		flags.world_density = json.get("world_density", 0).asUInt();
		flags.world_temperature = json.get("world_temperature", 0).asUInt();
		flags.world_composition = json.get("world_composition", 0).asUInt();
		flags.world_energy = json.get("world_energy", 0).asUInt();
		flags.world_chem_pot = json.get("world_chem_pot", 0).asUInt();
		flags.eintervdw = json.get("energy_intervdw", 0).asUInt();
		flags.eintravdw = json.get("energy_intravdw", 0).asUInt();
		flags.einterelect = json.get("energy_interelect", 0).asUInt();
		flags.eintraelect = json.get("energy_intraelect", 0).asUInt();
		flags.ebonded = json.get("energy_bonded", 0).asUInt();
		flags.econnectivity = json.get("energy_connectivity", 0).asUInt();
		flags.pideal = json.get("pressure_ideal", 0).asUInt();
		flags.pxx = json.get("pressure_pxx", 0).asUInt();
		flags.pxy = json.get("pressure_pxy", 0).asUInt();
		flags.pxz = json.get("pressure_pxz", 0).asUInt();
		flags.pyy = json.get("pressure_pyy", 0).asUInt();
		flags.pyz = json.get("pressure_pyz", 0).asUInt();
		flags.pzz = json.get("pressure_pzz", 0).asUInt();
		flags.ptail = json.get("pressure_tail", 0).asUInt();
		flags.hist_interval = json.get("hist_interval", 0).asUInt();
		flags.hist_bin_count = json.get("hist_bin_count", 0).asUInt();
		flags.hist_lower_outliers = json.get("hist_lower_outliers", 0).asUInt();
		flags.hist_upper_outliers = json.get("hist_upper_outliers", 0).asUInt();
		flags.hist_values = json.get("hist_values", 0).asUInt();
		flags.hist_counts = json.get("hist_counts", 0).asUInt();
		flags.particle_id = json.get("particle_id", 0).asUInt();
		flags.particle_species = json.get("particle_species", 0).asUInt();
		flags.particle_parent_id = json.get("particle_parent_id", 0).asUInt();
		flags.particle_parent_species = json.get("particle_parent_species", 0).asUInt();
		flags.particle_position = json.get("particle_position", 0).asUInt();
		flags.particle_director = json.get("particle_director", 0).asUInt();
		flags.particle_charge = json.get("particle_charge", 0).asUInt();

		// Masks.
		int simulation = json.get("simulation", 0).asInt();
		int world = json.get("world", 0).asInt();
		int energy_components = json.get("energy_components", 0).asInt();
		int pressure_tensor = json.get("pressure_tensor", 0).asInt();
		int histogram = json.get("histogram", 0).asInt();
		int particle = json.get("particle", 0).asInt();

		if(simulation)
			flags.simulation_on();
		if(world)
			flags.world_on();
		if(energy_components)
			flags.energy_on();
		if(pressure_tensor)
			flags.pressure_on();
		if(histogram)
			flags.histogram_on();
		if(particle)
			flags.particle_on();
		
		return flags;
	}

	void SimObserver::Update(const SimEvent& e)
	{
		// Only lock and proceed if we have to.
		if(e.GetIteration() % _frequency == 0 || e.ForceObserve())
		{
			_mutex.lock();
			_event = e;
			PreVisit();
			_event.GetObservable()->AcceptVisitor(*this);
			PostVisit();
			_mutex.unlock();
		}
	};

	SimObserver* SimObserver::BuildObserver(const Value &json)
	{
		return BuildObserver(json, "#/observers");
	}
	SimObserver* SimObserver::BuildObserver(const Value &json, const std::string& path)
	{
		ObjectRequirement validator; 
		Value schema;
		Reader reader;

		SimObserver* obs = nullptr;

		auto type = json.get("type", "none").asString();

		if(type == "DLMFile")
		{
			reader.parse(JsonSchema::DLMFileObserver, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			// Initialize flags.
			SimFlags flags = InitFlags(json["flags"]);

			// Initialize DLM specific options.
			auto prefix = json.get("prefix", "unnamed").asString();
			auto frequency = json.get("frequency", 1).asInt();
			auto colwidth = json.get("colwidth", 15).asInt();
			auto fixedw = json.get("fixedwmode", true).asBool();
			auto delimiter = json.get("delimiter", " ").asString();
			auto extension = json.get("extension", ".dat").asString();

			auto* dlm = new DLMFileObserver(prefix, flags, frequency);
			dlm->SetWidth(colwidth);
			dlm->SetFixedWidthMode(fixedw);
			dlm->SetDelimiter(delimiter);
			dlm->SetExtension(extension);
			obs = static_cast<SimObserver*>(dlm);
		}
		else if(type == "JSON")
		{
			reader.parse(JsonSchema::JSONObserver, schema);
			validator.Parse(schema, path);

			// Validate inputs. 
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			// Initialize flags.
			SimFlags flags;

			// Initialize JSON specific options.
			auto prefix = json.get("prefix", "unnamed").asString();
			auto frequency = json.get("frequency", 1).asInt();

			auto* dlm = new JSONObserver(prefix, flags, frequency);
			obs = static_cast<SimObserver*>(dlm);
		}
		else if(type == "XYZ")
		{
			reader.parse(JsonSchema::XYZObserver, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			// Initialize flags.
			SimFlags flags;

			// Initialize JSON specific options.
			auto prefix = json.get("prefix", "unnamed").asString();
			auto frequency = json.get("frequency", 1).asInt();

			auto* dlm = new XYZObserver(prefix, flags, frequency);
			obs = static_cast<SimObserver*>(dlm);
		}
		else
		{
			throw BuildException({path + ": Unknown observer type specified."});
		}

		return obs;
	}

	void SimObserver::BuildObservers(const Json::Value &json, ObserverList &ol)
	{
		ArrayRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::Observers, schema);
		validator.Parse(schema, "#/observers");

		// Validate high level schema.
		validator.Validate(json, "#/observers");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through observers.
		int i = 0;
		for(auto& obs : json)
		{
			ol.push_back(BuildObserver(obs, "#/observers/" + std::to_string(i)));
			++i;
		}
	}
}
