#include "World.h"
#include "SimpleWorld.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include <time.h>
#include <cstdlib>

using namespace Json;

namespace SAPHRON
{
	World* World::Build(std::istream& stream)
	{
		Reader reader;
		Value root;
		if(!reader.parse(stream, root))
			throw BuildException({reader.getFormattedErrorMessages()});

		return Build(root);
	}

	World* World::Build(const Value& json)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		World* world = nullptr;

		// Validation success.
		if(json["type"].asString() == "Simple")
		{
			// Parse schema.
			reader.parse(JsonSchema::SimpleWorld, schema);
			validator.Parse(schema, "#/worlds");

			// Validate input.
			validator.Validate(json, "#/worlds");

			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			Position dim{json["dimensions"][0].asDouble(), 
						 json["dimensions"][1].asDouble(),
						 json["dimensions"][2].asDouble()};

			// Neighbor list cutoff check.
			double ncut = json.get("nlist_cutoff",0.0).asDouble();
			if(ncut > dim[0]/2.0 || ncut > dim[1]/2.0 || ncut > dim[2]/2.0)
				throw BuildException({"Neighbor list cutoff must not exceed "
									  "half the shortest box vector."});
			
			// Skin thickness check.
			double rcut = json["r_cutoff"].asDouble();
			if(ncut && rcut > ncut)
				throw BuildException({"Cutoff radius must not exceed neighbor list cutoff."});

			srand(time(NULL));
			int seed = json.isMember("seed") ? json["seed"].asInt() : rand();

			
			world = new SimpleWorld(dim[0], dim[1], dim[2], rcut, seed);
			if(ncut)
				world->SetNeighborRadius(ncut);
		}

		// Initialize particles.
		if(json.isMember("particles"))
		{ 
			ParticleList particles;
			Particle::BuildParticles(json["particles"], json["components"], particles);
			
			for(auto& p : particles)
				world->AddParticle(p);
		}

		// Load temperature.
		world->SetTemperature(json.get("temperature", 0).asDouble());

		return world;
	}

	void World::AddParticleComposition(Particle* particle)
	{
		int id = particle->GetSpeciesID();
		if(_composition.find(id) == _composition.end())
			_composition[id] = 1;
		else
			++_composition[id];

		// If it's a primitive, add it to the primitives list. 
		if(!particle->HasChildren())
			_primitives.push_back(particle);

		for(auto& child : particle->GetChildren())
			AddParticleComposition(child);
	}

	void World::RemoveParticleComposition(Particle* particle)
	{
		int id = particle->GetSpeciesID();
		assert(_composition.find(id) != _composition.end());
		--_composition[id];

		if(!particle->HasChildren())
			_primitives.erase(
				std::remove(_primitives.begin(), _primitives.end(), particle),
				_primitives.end()
			);

		for(auto& child : particle->GetChildren())
			RemoveParticleComposition(child);
	}

	void World::ModifyParticleComposition(const ParticleEvent& pEvent)
	{
		int oldID = pEvent.GetOldSpecies();
		int id = pEvent.GetParticle()->GetSpeciesID();
		assert(_composition.find(oldID) != _composition.end());
		--_composition[oldID];

		if(_composition.find(id) == _composition.end())
			_composition[id] = 1;
		else
			++_composition[id];
	}

	void World::UpdateNeighborList()
	{
		int n = this->GetPrimitiveCount();

		// Clear neighbor list before repopulating.
		// We need to do this for parent particles
		// (it propogates to children) because parent 
		// checkpoints are used to check for nlist updates.
		#pragma omp parallel for
		for(int i = 0; i < this->GetParticleCount(); ++i)
		{
			_particles[i]->ClearNeighborList();
			_particles[i]->SetCheckpoint();
		}
		
		for(int i = 0; i < n - 1; ++i)
		{
			auto* pi = _primitives[i];
			const Position& posi = pi->GetPositionRef();
			
			for(int j = i + 1; j < n; ++j)
			{
				auto pj = _primitives[j];

				// Only add inter (non same molecule).
				if(pi->HasParent() && pj->HasParent() && 
					(pi->GetParent() == pj->GetParent()))
					continue;

				const Position& posj = pj->GetPositionRef();

				Position rij = posi - posj;
				ApplyMinimumImage(rij);

				if(arma::dot(rij,rij) <= _ncutsq)
				{
					pj->AddNeighbor(pi);
					pi->AddNeighbor(pj);
				}
			}
		}
	}

	void World::UpdateNeighborList(Particle* particle)
	{
		UpdateNeighborList(particle, true);
	}

	// Internal method. Allows for efficiency of clearing neighbor lists 
	// once.
	void World::UpdateNeighborList(Particle* particle, bool clear)
	{
		if(clear)
		{
			// These propogate to children.
			particle->RemoveFromNeighbors();
			particle->ClearNeighborList();
			particle->SetCheckpoint();
		}

		// If particle has no child update it.
		if(!particle->HasChildren())
		{
			const auto& pos = particle->GetPositionRef();
			for(size_t i = 0; i < _primitives.size(); ++i)
			{
				auto* pi = _primitives[i];

				// Only add inter (non same molecule).
				if(particle->HasParent() && pi->HasParent() && 
					(particle->GetParent() == pi->GetParent()))
					continue;

				// Skip self. 
				if(particle == pi)
					continue;

				const auto& posi = pi->GetPositionRef();

				Position rij = pos - posi;
				ApplyMinimumImage(rij);

				if(arma::dot(rij,rij) <= _ncutsq)
				{
					pi->AddNeighbor(particle);
					particle->AddNeighbor(pi);
				}
			}
		}
		
		for(auto& c : *particle)
			UpdateNeighborList(c, false);
	}

	// Initialize global world ID.
	int World::_nextID = 0;
}