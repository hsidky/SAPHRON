#include "World.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include <random>

using namespace Json;

namespace SAPHRON
{
	void World::AddParticleComposition(Particle* particle)
	{
		int id = particle->GetSpeciesID();

		// If ID doesn't exist yet, create it.
		if((int)_composition.size() - 1 < id)
			_composition.resize(id + 1, 0);

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
		assert(_composition.size() >= id);
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
		assert(_composition.size() >= id);
		--_composition[oldID];

		if((int)_composition.size() - 1 < id)
			_composition.resize(id + 1, 0);

		++_composition[id];
	}

	inline void World::AddNeighbor(Particle* pi, Particle* pj)
	{
		// Only add inter (non same molecule).
		if(pi->HasParent() && pj->HasParent() && 
		  (pi->GetParent() == pj->GetParent()))
			return;

		Position rij = pi->GetPosition() - pj->GetPosition();
		ApplyMinimumImage(&rij);

		if(fdot(rij,rij) <= _ncutsq)
		{
			pj->AddNeighbor(pi);
			pi->AddNeighbor(pj);
		}
	}

	inline void World::rect(int i0, int i1, int j0, int j1)
	{
		int di = i1 - i0;
		int dj = j1 - j0;
		constexpr int threshold = 32;
		if(di > threshold && dj > threshold)
		{
			int im = i0 + di/2;
			int jm = j0 + dj/2;
			#pragma omp task
			{ rect(i0, im, j0, jm); }
			rect(im, i1, jm, j1);
			#pragma omp taskwait
			
			#pragma omp task 
			{ rect(i0, im, jm, j1); }
			rect(im, i1, j0, jm);
			#pragma omp taskwait
		}
		else
		{
			for(int i = i0; i < i1; ++i)
				for(int j = j0; j < j1; ++j) 
				{
					auto* pi = _primitives[i];
					auto* pj = _primitives[j];
					AddNeighbor(pi, pj);
				}			
		}
	}

	inline void World::triangle(int n0, int n1)
	{
        int dn = n1 - n0;
		constexpr int threshold = 32;   
        if(dn > threshold)
        {
        	int nm = n0 + dn/2;
			#pragma omp task
        	{ triangle(n0, nm); }
			triangle(nm, n1);
			#pragma omp taskwait
     
			rect(n0, nm, nm, n1);
		}
		else
		{
			for(int i = n0; i < n1; ++i)
				for(int j = i+1; j < n1; ++j) 
				{
					auto* pi = _primitives[i];
					auto* pj = _primitives[j];
					AddNeighbor(pi, pj);
				}
		}
	}

	void World::UpdateNeighborList()
	{
		auto& sim = SimInfo::Instance();
		sim.StartTimer("nlist");

		int n = this->GetPrimitiveCount();

		// Clear neighbor list before repopulating.
		// We need to do this for parent particles
		// (it propogates to children) because parent 
		// checkpoints are used to check for nlist updates.
		#pragma omp parallel for schedule(static)
		for(int i = 0; i < this->GetParticleCount(); ++i)
		{
			_particles[i]->ClearNeighborList();
			_particles[i]->SetCheckpoint();
		}
		
		#pragma omp parallel
		#pragma omp single
		triangle(0, n);

		sim.AddTime("nlist");
	}

	void World::UpdateNeighborList(Particle* particle)
	{
		auto& sim = SimInfo::Instance();
		sim.StartTimer("nlist");

		UpdateNeighborList(particle, true);
		
		sim.AddTime("nlist");
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
			const auto& pos = particle->GetPosition();
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

				const auto& posi = pi->GetPosition();

				Position rij = pos - posi;
				ApplyMinimumImage(&rij);

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

	void World::PackWorld(const std::vector<Particle*>& particles,
		                  const std::vector<double>& fractions, 
		                  int n, double density)
	{

		if(particles.size() != fractions.size())
		{
			std::cerr << "ERROR: Particle and fraction count mismatch." << std::endl;
			exit(-1);
		}

		auto fracs = fractions;
		int nspecies = (int)particles.size();

		// Normalize
		double norm = 0;
		for(int i = 0; i < nspecies; i++)
			norm += fracs[i];

		for(int i = 0; i < nspecies; i++)
			fracs[i] /= norm;

		// Initialize counts.
		std::vector<int> counts(nspecies);
		for(int i = 0; i < nspecies-1; i++)
			counts[i] = (int)std::round(fracs[i]*n);

		// Make sure rounding above works out to perfect numbers.
		counts[nspecies-1] = n;
		for(int i = 0; i < nspecies-1; i++)
			counts[nspecies-1] -= counts[i];

		// New volume.
		double vn = (double)n/density;
		_H *= std::cbrt(vn/GetVolume());

		// Get corresponding box size.
		double L = std::cbrt(vn);

		// Find the lowest perfect cube greater than or equal to the number of particles.
		int nCube = 2;
		while(pow(nCube, 3) < n)
			++nCube;

		// Coordinates.
		int x = 0;
		int y = 0;
		int z = 0;

		// Assign particle positions.
		for (int i = 0; i < n; ++i)
		{
			for(int j = 0; j < nspecies; j++)
				if(counts[j] > 0 )
				{
					Particle* pnew = particles[j]->Clone();
					double mult = (L/nCube);
					Position pos = {mult*(x+0.5),mult*(y+0.5),mult*(z+0.5)};
					pnew->SetPosition(pos);
					AddParticle(pnew, false); // Update neighborlist later.
					--counts[j];
					break;
				}

			if(++x == nCube)
			{	
				x = 0;
				if(++y == nCube)
				{
					y = 0;
					++z;
				}
			}
		}

		UpdateNeighborList();
	}

	// Configure Particles in the lattice. For n particles and n fractions, 
	// the lattice will be initialized with the appropriate composition.
	void World::PackWorld(const std::vector<Particle*>& particles,
						  const std::vector<double>& fractions, int max)
	{
		if(particles.size() != fractions.size())
		{
			std::cerr << "ERROR: Particle and fraction count mismatch." << std::endl;
			exit(-1);
		}

		auto fracs = fractions;
		int cnt = (int)particles.size();

		// Normalize
		double norm = 0;
		for(int i = 0; i < cnt; i++)
			norm += fracs[i];

		for(int i = 0; i < cnt; i++)
			fracs[i] /= norm;

		// Initialize counts.
		std::vector<int> counts(cnt);
		for(int i = 0; i < cnt-1; i++)
			counts[i] = (int)std::round(fracs[i]*GetVolume());

		counts[cnt-1] = GetVolume();
		for(int i = 0; i < cnt-1; i++)
			counts[cnt-1] -= counts[i];

		// Loop though and initialize
		double x = 1;
		double y = 1;
		double z = 1;
		for(int i = 0; i < GetVolume(); i++)
		{
			if(z > _H(2,2))
				z = 1.0;
			if(y > _H(1,1))
				y = 1.0;
			if(x > _H(0,0))
				x = 1.0;

			for(int j = 0; j < cnt; j++)
				if(counts[j] > 0 )
				{
					Particle* pnew = particles[j]->Clone();
					pnew->SetPosition({x,y,z});
					AddParticle(pnew, false);
					--counts[j];
					break;
				}

			// Increment counters.
			x += floor((y + z) / (round(_H(2,2)) + round(_H(1,1))));
			y += floor(z / round(_H(2,2)));
			z += 1.0;

			if(max != 0 && i >= max - 1)
				return;
		}

		UpdateNeighborList();
	}

	void World::SetVolume(double v, bool scale)
	{
		auto l = pow(v, 1.0/3.0);

		if(scale)
		{
			auto xs = l/_H(0,0);
			auto ys = l/_H(1,1);
			auto zs = l/_H(2,2);

			_H(0,0) = l;
			_H(1,1) = l;
			_H(2,2) = l;

			#pragma omp parallel for schedule(static)
			for(auto it = _particles.begin(); it < _particles.end(); ++it)
			{
				auto pos = (*it)->GetPosition();
				pos[0] *= xs;
				pos[1] *= ys;
				pos[2] *= zs;
				ApplyPeriodicBoundaries(&pos);
				(*it)->SetPosition(pos);
			}

		}
		else
		{
			_H(0,0) = l;
			_H(1,1) = l;
			_H(2,2) = l;

			#pragma omp parallel for schedule(static)
			for(auto it = _particles.begin(); it < _particles.end(); ++it)
			{
				auto pos = (*it)->GetPosition();
				ApplyPeriodicBoundaries(&pos);
				(*it)->SetPosition(pos);
			}
		}
		
		// Regenerate neighbor list.
		UpdateNeighborList();
	}

	void World::Serialize(Json::Value& json) const
	{
		// TODO: Fix this.
		json["type"] = "Simple";
		json["temperature"] = this->GetTemperature();

		const auto& box = this->GetHMatrix();
		json["dimensions"][0] = box(0,0);
		json["dimensions"][1] = box(1,1);
		json["dimensions"][2] = box(2,2);

		// Serialize periodicity. 
		json["periodic"]["x"] = this->GetPeriodicX();
		json["periodic"]["y"] = this->GetPeriodicY();
		json["periodic"]["z"] = this->GetPeriodicZ();

		json["seed"] = this->GetSeed();
		json["skin_thickness"] = this->GetSkinThickness();
		json["nlist_cutoff"] = this->GetNeighborRadius();

		// Serialize chemical potentials.
		auto& slist = Particle::GetSpeciesList();
		for(size_t i = 0; i < _chemp.size(); ++i)
			if(_chemp[i] != 0) // Only write non-zero chemical potentials?
				json["chemical_potential"][slist[i]] = _chemp[i];

		// TODO: find a better way to do this. 
		// Group master particles by species.
		std::map<std::string, ParticleList> plist; 
		for(auto& p : _particles)
		{
			auto s = p->GetSpecies();
			if(plist.find(s) == plist.end())
				plist[s] = {p};
			else
				plist[s].push_back(p);
		}

		// Go through master particles and serialize primitives.
		// Generate components as well.
		auto& components = json["components"];
		Json::Value comp, prop;
		for(auto& pair : plist)
		{
			comp[0] = pair.first;
			comp[1] = (int)pair.second.size();
			components.append(comp);
			for(auto& p : pair.second)
			{
				if(p->HasChildren())
				{
					for(auto& child : *p)
					{
						child->Serialize(prop);
						json["particles"].append(prop);
					}
				}
				else
				{
					p->Serialize(prop);
					json["particles"].append(prop);
				}
			}
		}
	}

	World* World::Build(const Value& json,
						const Value& blueprints)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		World* world = nullptr;

		// TODO: there's only a simple world. Update this and schema name.
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
		double ncut = json["nlist_cutoff"].asDouble();
		if(ncut > dim[0]/2.0 || ncut > dim[1]/2.0 || ncut > dim[2]/2.0)
			throw BuildException({"Neighbor list cutoff must not exceed "
								  "half the shortest box vector."});
		
		// Skin thickness check.
		double skin = json["skin_thickness"].asDouble();
		if(ncut && skin > ncut)
			throw BuildException({"Skin thickness must not exceed neighbor list cutoff."});

		// Get seed.
		std::random_device rd;
		unsigned seed = json.get("seed", rd()).asUInt();

		world = new World(dim[0], dim[1], dim[2], ncut, skin, seed);
		if(ncut)
			world->SetNeighborRadius(ncut);
		
		// Periodic. 
		bool periodx = true, periody = true, periodz = true;
		if(json.isMember("periodic"))
		{
			periodx = json["periodic"].get("x", true).asBool();
			periody = json["periodic"].get("y", true).asBool();
			periodz = json["periodic"].get("z", true).asBool();
		}
		world->SetPeriodicX(periodx);
		world->SetPeriodicY(periody);
		world->SetPeriodicZ(periodz);			

		// Initialize particles.
		if(json.isMember("particles"))
		{ 
			ParticleList particles;
			Particle::BuildParticles(json["particles"], 
									 blueprints,
									 json["components"], 
									 particles);
			
			// Use these to pack the box. 
			if(json.isMember("pack"))
			{
				auto count = json["pack"]["count"].asInt();
				auto rho = json["pack"].get("density", 0.0).asDouble();
				
				// We expect a single particle prototype 
				// for each species defined in composition.
				auto& comp = json["pack"]["composition"];
				if(comp.size() != particles.size())
					throw BuildException({"Expected " + std::to_string(comp.size()) + 
						" particles to pack but " + std::to_string(particles.size()) + 
						" were defined."});

				// Get compositions.
				std::vector<double> xi;
				for(auto& p : particles)
				{
					auto s = p->GetSpecies();
					if(!comp.isMember(s))
						throw BuildException({"Particle of type " + s + " defined " + 
							"but no composition specified."});

					xi.push_back(comp[s].asDouble());
				}

				// Pack world. If rho is zero, then we pack it with whatever,
				// and force the volume specified by the user afterwards.
				if(rho == 0)
				{
					auto V = world->GetVolume();
					world->PackWorld(particles, xi, count, 1.0);
					world->SetVolume(V, true);
				}
				else
					world->PackWorld(particles, xi, count, rho);

				// Clean up our mess.
				for(auto& p : particles)
					delete p;

				particles.clear();
			}
			else 
			{
				for(auto& p : particles)
				{
					// Apply periodic boundary conditions.
					auto pos = p->GetPosition();
					world->ApplyPeriodicBoundaries(&pos);
					p->SetPosition(pos);
					world->AddParticle(p);
				}
			}
		}

		// Load temperature.
		world->SetTemperature(json.get("temperature", 0).asDouble());

		// Load chemical potentials. 
		if(json.isMember("chemical_potential"))
		{
			auto& chemp = json["chemical_potential"];
			for(auto& species : chemp.getMemberNames())
				world->SetChemicalPotential(species, chemp[species].asDouble());
		}

		return world;
	}

	// Initialize global world ID.
	int World::_nextID = 0;
}